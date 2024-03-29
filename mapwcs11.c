/******************************************************************************
 * $Id: mapwcs11.c 11503 2011-04-07 19:56:16Z dmorissette $
 *
 * Project:  MapServer
 * Purpose:  OpenGIS Web Coverage Server (WCS) 1.1.0 Implementation.  This
 *           file holds some WCS 1.1.0 specific functions but other parts
 *           are still implemented in mapwcs.c.
 * Author:   Frank Warmerdam and the MapServer team.
 *
 ******************************************************************************
 * Copyright (c) 2007, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies of this Software or works derived from this Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#include <assert.h>
#include "mapserver.h"
#include "maperror.h"
#include "mapthread.h"
#include "mapwcs.h"

MS_CVSID("$Id: mapwcs11.c 11503 2011-04-07 19:56:16Z dmorissette $")

#if defined(USE_WCS_SVR) && defined(USE_LIBXML2)

#include "mapwcs.h"
#include "maplibxml2.h"
#include "gdal.h"
#include "cpl_string.h" /* GDAL string handling */

/*
** msWCSException11()
**
** Report current MapServer error in XML exception format.
** Wrapper function around msOWSCommonExceptionReport. Merely
** passes WCS specific info.
** 
*/

int msWCSException11(mapObj *map, const char *locator, 
                     const char *exceptionCode, const char *version) {
  int size = 0;
  char *errorString     = NULL;
  char *errorMessage    = NULL;
  char *schemasLocation = NULL;
  const char * encoding;

  xmlDocPtr  psDoc      = NULL;   
  xmlNodePtr psRootNode = NULL;
  xmlNsPtr   psNsOws    = NULL;
  xmlChar *buffer       = NULL;

  psNsOws = xmlNewNs(NULL, BAD_CAST "http://www.opengis.net/ows/1.1", BAD_CAST "ows");

  encoding = msOWSLookupMetadata(&(map->web.metadata), "CO", "encoding");
  errorString = msGetErrorString("\n");
  errorMessage = msEncodeHTMLEntities(errorString);
  schemasLocation = msEncodeHTMLEntities(msOWSGetSchemasLocation(map));

  psDoc = xmlNewDoc(BAD_CAST "1.0");

  psRootNode = msOWSCommonExceptionReport(psNsOws, OWS_1_1_0, schemasLocation, version, msOWSGetLanguage(map, "exception"), exceptionCode, locator, errorMessage);

  xmlDocSetRootElement(psDoc, psRootNode);

  psNsOws = xmlNewNs(psRootNode, BAD_CAST "http://www.opengis.net/ows/1.1", BAD_CAST "ows");

  if (encoding)
      msIO_printf("Content-type: text/xml; charset=%s%c%c", encoding,10,10);
  else
      msIO_printf("Content-type: text/xml%c%c",10,10);

  xmlDocDumpFormatMemoryEnc(psDoc, &buffer, &size, (encoding ? encoding : "ISO-8859-1"), 1);
    
  msIO_printf("%s", buffer);

  /*free buffer and the document */
  free(errorString);
  free(errorMessage);
  free(schemasLocation);
  xmlFree(buffer);
  xmlFreeDoc(psDoc);

  /* clear error since we have already reported it */
  msResetErrorList();

  return MS_FAILURE;
}

/************************************************************************/
/*                       msWCSGetFormatsList11()                        */
/*                                                                      */
/*      Fetch back a comma delimited formats list for the past layer    */
/*      if one is supplied, otherwise for all formats supported by      */
/*      the server.  Formats should be identified by mime type.         */
/************************************************************************/

static char *msWCSGetFormatsList11( mapObj *map, layerObj *layer )

{
    char *format_list = msStrdup("");
    char **tokens = NULL, **formats = NULL;
    int  i, numtokens = 0, numformats;
    const char *value;

    msApplyDefaultOutputFormats(map);

/* -------------------------------------------------------------------- */
/*      Parse from layer metadata.                                      */
/* -------------------------------------------------------------------- */
    if( layer != NULL 
        && (value = msOWSGetEncodeMetadata( &(layer->metadata),"CO","formats",
                                            "GTiff" )) != NULL ) {
        tokens = msStringSplit(value, ' ', &numtokens);
    }

/* -------------------------------------------------------------------- */
/*      Or generate from all configured raster output formats that      */
/*      look plausible.                                                 */
/* -------------------------------------------------------------------- */
    else
    {
        tokens = (char **) calloc(map->numoutputformats,sizeof(char*));
        for( i = 0; i < map->numoutputformats; i++ )
        {
            switch( map->outputformatlist[i]->renderer )
            {
                /* seeminly normal raster format */
              case MS_RENDER_WITH_GD:
              case MS_RENDER_WITH_AGG:
              case MS_RENDER_WITH_RAWDATA:
                tokens[numtokens++] = msStrdup(map->outputformatlist[i]->name);
                break;
                
                /* rest of formats aren't really WCS compatible */
              default:
                break;
                
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Convert outputFormatObj names into mime types and remove        */
/*      duplicates.                                                     */
/* -------------------------------------------------------------------- */
    numformats = 0;
    formats = (char **) calloc(sizeof(char*),numtokens);
    
    for( i = 0; i < numtokens; i++ )
    {
        int format_i, j;
        const char *mimetype;
        
        for( format_i = 0; format_i < map->numoutputformats; format_i++ )
        {
            if( strcasecmp(map->outputformatlist[format_i]->name,
                           tokens[i]) == 0 )
                break;
        }
        

        if( format_i == map->numoutputformats )
        {
            msDebug("Failed to find outputformat info on format '%s', ignore.\n",
                    tokens[i] );
            continue;
        }

        mimetype = map->outputformatlist[format_i]->mimetype;
        if( mimetype == NULL || strlen(mimetype) == 0 )
        {
            msDebug("No mimetime for format '%s', ignoring.\n",
                    tokens[i] );
            continue;
        }
        
        for( j = 0; j < numformats; j++ )
        {
            if( strcasecmp(mimetype,formats[j]) == 0 )
                break;
        }

        if( j < numformats )
        {
            msDebug( "Format '%s' ignored since mimetype '%s' duplicates another outputFormatObj.\n", 
                     tokens[i], mimetype );
            continue;
        }
        
        formats[numformats++] = msStrdup(mimetype);
    }

    msFreeCharArray(tokens,numtokens);

/* -------------------------------------------------------------------- */
/*      Turn mimetype list into comma delimited form for easy use       */
/*      with xml functions.                                             */
/* -------------------------------------------------------------------- */
    for(i=0; i<numformats; i++) 
    {
        if(i > 0)
        {
            format_list = msStringConcatenate(format_list, (char *) ",");
        }
        format_list = msStringConcatenate(format_list, formats[i]);
    }
    msFreeCharArray(formats,numformats);

    return format_list;
}

/************************************************************************/
/*                msWCSGetCapabilities11_CoverageSummary()              */
/*                                                                      */
/*      Generate a WCS 1.1 CoverageSummary.                             */
/************************************************************************/

static int msWCSGetCapabilities11_CoverageSummary(
    mapObj *map, wcsParamsObj *params, cgiRequestObj *req, 
    xmlDocPtr doc, xmlNodePtr psContents, layerObj *layer )

{
    coverageMetadataObj cm;
    int status;
    const char *value;
    char *owned_value;
    char *format_list;
    xmlNodePtr psCSummary;
    xmlNsPtr psOwsNs = xmlSearchNs( doc, psContents, BAD_CAST "ows" );
    char **tokens = NULL;
    int i = 0;
    int n = 0;

    status = msWCSGetCoverageMetadata(layer, &cm);
    if(status != MS_SUCCESS) return MS_FAILURE;

    psCSummary = xmlNewChild( psContents, NULL, BAD_CAST "CoverageSummary", NULL );

/* -------------------------------------------------------------------- */
/*      Title (from description)                                        */
/* -------------------------------------------------------------------- */
    value = msOWSLookupMetadata( &(layer->metadata), "CO", "description");
    if( value == NULL )
        value = msOWSLookupMetadata( &(layer->metadata), "CO", "title");
        if( value == NULL )
            value = layer->name;
    xmlNewChild( psCSummary, psOwsNs, BAD_CAST "Title", BAD_CAST value );

/* -------------------------------------------------------------------- */
/*      Abstract                                                        */
/* -------------------------------------------------------------------- */
    value = msOWSLookupMetadata( &(layer->metadata), "CO", "abstract");
    xmlNewChild( psCSummary, psOwsNs, BAD_CAST "Abstract", BAD_CAST value );

/* -------------------------------------------------------------------- */
/*      Keywords                                                        */
/* -------------------------------------------------------------------- */
    value = msOWSLookupMetadata(&(layer->metadata), "CO", "keywordlist");

    if (value) {
        xmlNodePtr psNode;

        psNode = xmlNewChild(psCSummary, psOwsNs, BAD_CAST "Keywords", NULL);

        tokens = msStringSplit(value, ',', &n);
        if (tokens && n > 0) {
            for (i=0; i<n; i++) {
                xmlNewChild(psNode, NULL, BAD_CAST "Keyword", BAD_CAST tokens[i] );
            }
            msFreeCharArray(tokens, n);
        }
    }

/* -------------------------------------------------------------------- */
/*      WGS84 bounding box.                                             */
/* -------------------------------------------------------------------- */
    xmlAddChild( 
        psCSummary,
        msOWSCommonWGS84BoundingBox( psOwsNs, 2,
                                     cm.llextent.minx, cm.llextent.miny,
                                     cm.llextent.maxx, cm.llextent.maxy ));

/* -------------------------------------------------------------------- */
/*      Supported CRSes.                                                */
/* -------------------------------------------------------------------- */
    if( (owned_value = 
         msOWSGetProjURN( &(layer->projection), &(layer->metadata), 
                          "CO", MS_FALSE)) != NULL ) {
        /* ok */
    } else if((owned_value = 
               msOWSGetProjURN( &(layer->map->projection), 
                                &(layer->map->web.metadata), 
                                "CO", MS_FALSE)) != NULL ) {
        /* ok */
    } else 
        msDebug( "mapwcs.c: missing required information, no SRSs defined.\n");
    
    if( owned_value != NULL && strlen(owned_value) > 0 ) 
        msLibXml2GenerateList( psCSummary, NULL, "SupportedCRS", 
                                owned_value, ' ' );

    msFree( owned_value );

/* -------------------------------------------------------------------- */
/*      SupportedFormats                                                */
/* -------------------------------------------------------------------- */
    format_list = msWCSGetFormatsList11( map, layer );

    if (strlen(format_list) > 0 )
        msLibXml2GenerateList( psCSummary, NULL, "SupportedFormat",
                                format_list, ',' );

    msFree( format_list );

/* -------------------------------------------------------------------- */
/*      Identifier (layer name)                                         */
/* -------------------------------------------------------------------- */
    xmlNewChild( psCSummary, NULL, BAD_CAST "Identifier", BAD_CAST layer->name );
  
    return MS_SUCCESS;
}

/************************************************************************/
/*                       msWCSGetCapabilities11()                       */
/************************************************************************/
int msWCSGetCapabilities11(mapObj *map, wcsParamsObj *params, 
                           cgiRequestObj *req, owsRequestObj *ows_request)
{
    xmlDocPtr psDoc = NULL;       /* document pointer */
    xmlNodePtr psRootNode, psMainNode, psNode;
    xmlNodePtr psTmpNode;
    char *identifier_list = NULL, *format_list = NULL;
    const char *updatesequence=NULL;
    const char *encoding;
    xmlNsPtr psOwsNs, psXLinkNs;
    char *schemaLocation = NULL;
    char *xsi_schemaLocation = NULL;
    char *script_url=NULL, *script_url_encoded=NULL;

    xmlChar *buffer = NULL;
    int size = 0, i;
    msIOContext *context = NULL;

    int ows_version = OWS_1_1_0;

/* -------------------------------------------------------------------- */
/*      Handle updatesequence                                           */
/* -------------------------------------------------------------------- */

    updatesequence = msOWSLookupMetadata(&(map->web.metadata), "CO", "updatesequence");
    encoding = msOWSLookupMetadata(&(map->web.metadata), "CO", "encoding");

    if (params->updatesequence != NULL) {
      i = msOWSNegotiateUpdateSequence(params->updatesequence, updatesequence);
      if (i == 0) { /* current */
          msSetError(MS_WCSERR, "UPDATESEQUENCE parameter (%s) is equal to server (%s)", "msWCSGetCapabilities11()", params->updatesequence, updatesequence);
          return msWCSException11(map, "updatesequence", "CurrentUpdateSequence", params->version);
      }
      if (i > 0) { /* invalid */
          msSetError(MS_WCSERR, "UPDATESEQUENCE parameter (%s) is higher than server (%s)", "msWCSGetCapabilities11()", params->updatesequence, updatesequence);
          return msWCSException11(map, "updatesequence", "InvalidUpdateSequence", params->version);
      }
    }

/* -------------------------------------------------------------------- */
/*      Build list of layer identifiers available.                      */
/* -------------------------------------------------------------------- */
    identifier_list = msStrdup("");
    for(i=0; i<map->numlayers; i++)
    {
        layerObj *layer = map->layers[i];
        int       new_length;

        if(!msWCSIsLayerSupported(layer)) 
            continue;

        new_length = strlen(identifier_list) + strlen(layer->name) + 2;
        identifier_list = (char *) realloc(identifier_list,new_length);

        if( strlen(identifier_list) > 0 )
            strcat( identifier_list, "," );
        strcat( identifier_list, layer->name );
    }

/* -------------------------------------------------------------------- */
/*      Create document.                                                */
/* -------------------------------------------------------------------- */
    psDoc = xmlNewDoc(BAD_CAST "1.0");

    psRootNode = xmlNewNode(NULL, BAD_CAST "Capabilities");

    xmlDocSetRootElement(psDoc, psRootNode);

/* -------------------------------------------------------------------- */
/*      Name spaces                                                     */
/* -------------------------------------------------------------------- */
    xmlSetNs(psRootNode, xmlNewNs(psRootNode, BAD_CAST "http://www.opengis.net/wcs/1.1", NULL));
    psOwsNs = xmlNewNs(psRootNode, BAD_CAST MS_OWSCOMMON_OWS_110_NAMESPACE_URI, BAD_CAST MS_OWSCOMMON_OWS_NAMESPACE_PREFIX);
    psXLinkNs = xmlNewNs(psRootNode, BAD_CAST MS_OWSCOMMON_W3C_XLINK_NAMESPACE_URI, BAD_CAST MS_OWSCOMMON_W3C_XLINK_NAMESPACE_PREFIX);
    xmlNewNs(psRootNode, BAD_CAST MS_OWSCOMMON_W3C_XSI_NAMESPACE_URI, BAD_CAST MS_OWSCOMMON_W3C_XSI_NAMESPACE_PREFIX);
    xmlNewNs(psRootNode, BAD_CAST MS_OWSCOMMON_OGC_NAMESPACE_URI, BAD_CAST MS_OWSCOMMON_OGC_NAMESPACE_PREFIX );

    /*xmlNewProp(psRootNode, BAD_CAST " */
    xmlNewProp(psRootNode, BAD_CAST "version", BAD_CAST params->version );

    updatesequence = msOWSLookupMetadata(&(map->web.metadata), "CO", "updatesequence");

    if (updatesequence)
      xmlNewProp(psRootNode, BAD_CAST "updateSequence", BAD_CAST updatesequence);

    schemaLocation = msEncodeHTMLEntities( msOWSGetSchemasLocation(map) );
    xsi_schemaLocation = msStrdup("http://www.opengis.net/wcs/1.1");
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, " ");
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, schemaLocation);
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, "/wcs/1.1/wcsGetCapabilities.xsd ");
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, MS_OWSCOMMON_OWS_110_NAMESPACE_URI);
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, " ");
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, schemaLocation);
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, "/ows/1.1.0/owsAll.xsd");
    xmlNewNsProp(psRootNode, NULL, BAD_CAST "xsi:schemaLocation", BAD_CAST xsi_schemaLocation);

/* -------------------------------------------------------------------- */
/*      Service metadata.                                               */
/* -------------------------------------------------------------------- */
    if( params->section == NULL 
        || strstr(params->section,"All") != NULL
        || strstr(params->section,"ServiceIdentification") != NULL )
    {
        psTmpNode = xmlAddChild(psRootNode, msOWSCommonServiceIdentification(
                                    psOwsNs, map, "OGC WCS", params->version, "CO"));
    }

    /*service provider*/
    if( params->section == NULL 
        || strstr(params->section,"All") != NULL
        || strstr(params->section,"ServiceProvider") != NULL )
    {
        psTmpNode = xmlAddChild(psRootNode, msOWSCommonServiceProvider(
                                    psOwsNs, psXLinkNs, map, "CO"));
    }

/* -------------------------------------------------------------------- */
/*      Operations metadata.                                            */
/* -------------------------------------------------------------------- */
    /*operation metadata */
    if ((script_url=msOWSGetOnlineResource(map, "CO", "onlineresource", req)) == NULL 
        || (script_url_encoded = msEncodeHTMLEntities(script_url)) == NULL)
    {
        msSetError(MS_WCSERR, "Server URL not found", "msWCSGetCapabilities11()");
        return msWCSException11(map, "mapserv", "NoApplicableCode", params->version);
    }
    free( script_url );

    if( params->section == NULL 
        || strstr(params->section,"All") != NULL
        || strstr(params->section,"OperationsMetadata") != NULL )
    {
        psMainNode= xmlAddChild(psRootNode,msOWSCommonOperationsMetadata(psOwsNs));

/* -------------------------------------------------------------------- */
/*      GetCapabilities - add Sections and AcceptVersions?              */
/* -------------------------------------------------------------------- */
        psNode = msOWSCommonOperationsMetadataOperation( 
            psOwsNs, psXLinkNs,
            "GetCapabilities", OWS_METHOD_GETPOST, script_url_encoded);
        
        xmlAddChild(psMainNode, psNode);
        xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                        ows_version, psOwsNs, "Parameter", "service", "WCS"));
        xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                        ows_version, psOwsNs, "Parameter", "version", (char *)params->version));

/* -------------------------------------------------------------------- */
/*      DescribeCoverage                                                */
/* -------------------------------------------------------------------- */
        if (msOWSRequestIsEnabled(map, NULL, "C", "DescribeCoverage", MS_TRUE)) 
        {
            psNode = msOWSCommonOperationsMetadataOperation(
                psOwsNs, psXLinkNs,
                "DescribeCoverage", OWS_METHOD_GETPOST, script_url_encoded);
            
            xmlAddChild(psMainNode, psNode);
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "service", "WCS"));
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "version", (char *)params->version));
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "identifiers", identifier_list ));
        }
        
/* -------------------------------------------------------------------- */
/*      GetCoverage                                                     */
/* -------------------------------------------------------------------- */
        if (msOWSRequestIsEnabled(map, NULL, "C", "GetCoverage", MS_TRUE)) 
        {

            psNode = msOWSCommonOperationsMetadataOperation(
                psOwsNs, psXLinkNs,
                "GetCoverage", OWS_METHOD_GETPOST, script_url_encoded);
            
            format_list = msWCSGetFormatsList11( map, NULL );
            
            xmlAddChild(psMainNode, psNode);
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "service", "WCS"));
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "version", (char *)params->version));
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "Identifier", identifier_list ));
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "InterpolationType", 
                            "NEAREST_NEIGHBOUR,BILINEAR" ));
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "format", format_list ));
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "store", "false" ));
            xmlAddChild(psNode, msOWSCommonOperationsMetadataDomainType(
                            ows_version, psOwsNs, "Parameter", "GridBaseCRS", 
                            "urn:ogc:def:crs:epsg::4326" ));
        
            msFree( format_list );
        }
    }    

/* -------------------------------------------------------------------- */
/*      Contents section.                                               */
/* -------------------------------------------------------------------- */
    if( params->section == NULL 
        || strstr(params->section,"All") != NULL
        || strstr(params->section,"Contents") != NULL )
    {
        psMainNode = xmlNewChild( psRootNode, NULL, BAD_CAST "Contents", NULL );

        for(i=0; i<map->numlayers; i++)
        {
            layerObj *layer = map->layers[i];
            int       status;
            
            if(!msWCSIsLayerSupported(layer)) 
                continue;
            
            if (!msIntegerInArray(layer->index, ows_request->enabled_layers, ows_request->numlayers))
                continue;

            status = msWCSGetCapabilities11_CoverageSummary( 
                map, params, req, psDoc, psMainNode, layer );
            if(status != MS_SUCCESS) return MS_FAILURE;
        }
    }

/* -------------------------------------------------------------------- */
/*      Write out the document.                                         */
/* -------------------------------------------------------------------- */

    if( msIO_needBinaryStdout() == MS_FAILURE )
        return MS_FAILURE;
     
    if (encoding)
        msIO_printf("Content-type: text/xml; charset=%s%c%c", encoding,10,10);
    else
        msIO_printf("Content-type: text/xml%c%c",10,10);
    
    context = msIO_getHandler(stdout);

    xmlDocDumpFormatMemoryEnc(psDoc, &buffer, &size, (encoding ? encoding : "ISO-8859-1"), 1);
    msIO_contextWrite(context, buffer, size);
    xmlFree(buffer);

    /*free buffer and the document */
    /*xmlFree(buffer);*/
    xmlFreeDoc(psDoc);

    xmlCleanupParser();

    /* clean up */
    free( script_url_encoded );
    free( identifier_list );

    return(MS_SUCCESS);
}

/************************************************************************/
/*            msWCSDescribeCoverage_CoverageDescription11()             */
/************************************************************************/

static int 
msWCSDescribeCoverage_CoverageDescription11(
    layerObj *layer, wcsParamsObj *params, xmlNodePtr psRootNode,
    xmlNsPtr psOwsNs )

{
    int status;
    coverageMetadataObj cm;
    xmlNodePtr psCD, psDomain, psSD, psGridCRS;
    const char *value;

/* -------------------------------------------------------------------- */
/*      Verify layer is processable.                                    */
/* -------------------------------------------------------------------- */
    if( msCheckParentPointer(layer->map,"map") == MS_FAILURE )
	return MS_FAILURE;

    if(!msWCSIsLayerSupported(layer)) 
        return MS_SUCCESS;
    
/* -------------------------------------------------------------------- */
/*      Setup coverage metadata.                                        */
/* -------------------------------------------------------------------- */
    status = msWCSGetCoverageMetadata(layer, &cm);
    if(status != MS_SUCCESS) return status;

    /* fill in bands rangeset info, if required.  */
    msWCSSetDefaultBandsRangeSetInfo( params, &cm, layer );

/* -------------------------------------------------------------------- */
/*      Create CoverageDescription node.                                */
/* -------------------------------------------------------------------- */
    psCD = xmlNewChild( psRootNode, NULL, BAD_CAST "CoverageDescription", NULL );
    
/* -------------------------------------------------------------------- */
/*      Title (from description)                                        */
/* -------------------------------------------------------------------- */
    value = msOWSLookupMetadata( &(layer->metadata), "CO", "description");
    if( value == NULL )
        value = layer->name;
    xmlNewChild( psCD, psOwsNs, BAD_CAST "Title", BAD_CAST value );

/* -------------------------------------------------------------------- */
/*      Abstract                                                        */
/* -------------------------------------------------------------------- */
    value = msOWSLookupMetadata( &(layer->metadata), "CO", "abstract");
    xmlNewChild( psCD, psOwsNs, BAD_CAST "Abstract", BAD_CAST value );

/* -------------------------------------------------------------------- */
/*      Keywords                                                        */
/* -------------------------------------------------------------------- */
    value = msOWSLookupMetadata(&(layer->metadata), "CO", "keywordlist");

    if (value)
        msLibXml2GenerateList( 
            xmlNewChild(psCD, psOwsNs, BAD_CAST "Keywords", NULL),
            NULL, "Keyword", value, ',' );

/* -------------------------------------------------------------------- */
/*      Identifier (layer name)                                         */
/* -------------------------------------------------------------------- */
    xmlNewChild( psCD, NULL, BAD_CAST "Identifier", BAD_CAST layer->name);

/* -------------------------------------------------------------------- */
/*      Domain                                                          */
/* -------------------------------------------------------------------- */
    psDomain = xmlNewChild( psCD, NULL, BAD_CAST "Domain", NULL );

/* -------------------------------------------------------------------- */
/*      SpatialDomain                                                   */
/* -------------------------------------------------------------------- */
    psSD = xmlNewChild( psDomain, NULL, BAD_CAST "SpatialDomain", NULL );

/* -------------------------------------------------------------------- */
/*      imageCRS bounding box.                                          */
/* -------------------------------------------------------------------- */
    xmlAddChild( 
        psSD,
        msOWSCommonBoundingBox( psOwsNs, "urn:ogc:def:crs:OGC::imageCRS",
                                2, 0, 0, cm.xsize-1, cm.ysize-1 ));

/* -------------------------------------------------------------------- */
/*      native CRS bounding box.                                        */
/* -------------------------------------------------------------------- */
    xmlAddChild( 
        psSD,
        msOWSCommonBoundingBox( psOwsNs, cm.srs_urn, 2, 
                                cm.extent.minx, cm.extent.miny,
                                cm.extent.maxx, cm.extent.maxy ));

/* -------------------------------------------------------------------- */
/*      WGS84 bounding box.                                             */
/* -------------------------------------------------------------------- */
    xmlAddChild( 
        psSD,
        msOWSCommonWGS84BoundingBox( psOwsNs, 2,
                                     cm.llextent.minx, cm.llextent.miny,
                                     cm.llextent.maxx, cm.llextent.maxy ));

/* -------------------------------------------------------------------- */
/*      GridCRS                                                         */
/* -------------------------------------------------------------------- */
    {
        char format_buf[500];

        psGridCRS = xmlNewChild( psSD, NULL, BAD_CAST "GridCRS", NULL );

        
        xmlNewChild( psGridCRS, NULL, BAD_CAST "GridBaseCRS", BAD_CAST cm.srs_urn );
        xmlNewChild( psGridCRS, NULL, BAD_CAST "GridType", 
                     BAD_CAST "urn:ogc:def:method:WCS:1.1:2dSimpleGrid" );

        sprintf( format_buf, "%.15g %.15g", 
                 cm.geotransform[0]+cm.geotransform[1]/2+cm.geotransform[2]/2, 
                 cm.geotransform[3]+cm.geotransform[4]/2+cm.geotransform[5]/2);
        xmlNewChild( psGridCRS, NULL, BAD_CAST "GridOrigin", BAD_CAST format_buf );

        sprintf( format_buf, "%.15g %.15g", 
                 cm.geotransform[1], cm.geotransform[5] );
        xmlNewChild( psGridCRS, NULL, BAD_CAST "GridOffsets", BAD_CAST format_buf );

        xmlNewChild( psGridCRS, NULL, BAD_CAST "GridCS", 
                     BAD_CAST "urn:ogc:def:cs:OGC:0.0:Grid2dSquareCS" );
    }



#ifdef notdef
  /* TemporalDomain */

  /* TODO: figure out when a temporal domain is valid, for example only tiled rasters support time as a domain, plus we need a timeitem */
  if(msOWSLookupMetadata(&(layer->metadata), "CO", "timeposition") || msOWSLookupMetadata(&(layer->metadata), "CO", "timeperiod")) {
    msIO_printf("      <temporalDomain>\n");

    /* TimePosition (should support a value AUTO, then we could mine positions from the timeitem) */
    msOWSPrintEncodeMetadataList(stdout, &(layer->metadata), "CO", "timeposition", NULL, NULL, "        <gml:timePosition>%s</gml:timePosition>\n", NULL);    

    /* TODO:  add TimePeriod (only one per layer)  */

    msIO_printf("      </temporalDomain>\n");
  }
  
  msIO_printf("    </domainSet>\n");
#endif

/* -------------------------------------------------------------------- */
/*      Range                                                           */
/* -------------------------------------------------------------------- */
    {
        xmlNodePtr psField, psInterpMethods, psAxis, psDefinition;
        const char *value;

        psField = 
            xmlNewChild(
                xmlNewChild( psCD, NULL, BAD_CAST "Range", NULL ),
                NULL, BAD_CAST "Field", NULL );
        
        value = msOWSGetEncodeMetadata( &(layer->metadata), "CO", 
                                        "rangeset_label", NULL );
        if( value )
            xmlNewChild( psField, psOwsNs, BAD_CAST "Title", BAD_CAST value );

        /* ows:Abstract? TODO */

        value = msOWSGetEncodeMetadata( &(layer->metadata), "CO", 
                                        "rangeset_name", "raster" );
        xmlNewChild( psField, NULL, BAD_CAST "Identifier", BAD_CAST value );
       
        psDefinition =
            xmlNewChild(
                xmlNewChild( psField, NULL, BAD_CAST "Definition", NULL ),
                psOwsNs, BAD_CAST "AnyValue", NULL );

        /* NullValue */
        value = msOWSGetEncodeMetadata( &(layer->metadata), "CO", 
                                        "rangeset_nullvalue", NULL);
        if( value )
            xmlNewChild( psField, NULL, BAD_CAST "NullValue", 
                         BAD_CAST value );

        /* InterpolationMethods */
        psInterpMethods = 
            xmlNewChild( psField, NULL, BAD_CAST "InterpolationMethods", NULL );

        xmlNewChild( psInterpMethods, NULL, BAD_CAST "InterpolationMethod", BAD_CAST "bilinear" );
        xmlNewChild( psInterpMethods, NULL, 
                     BAD_CAST "Default", BAD_CAST "nearest neighbor" );

/* -------------------------------------------------------------------- */
/*      Bands axis.                                                     */
/* -------------------------------------------------------------------- */
        {
            xmlNodePtr psKeys;
            int iBand;

            value = msOWSGetEncodeMetadata( &(layer->metadata), "CO", 
                                            "bands_name", "bands" );
            psAxis = xmlNewChild( psField, NULL, BAD_CAST "Axis", NULL );
            xmlNewProp( psAxis, BAD_CAST "identifier", BAD_CAST value );
            
            psKeys = xmlNewChild( psAxis, NULL, BAD_CAST 
                                  "AvailableKeys",  NULL );
            
            for( iBand = 0; iBand < cm.bandcount; iBand++ )
            {
                char szBandName[32];

                snprintf( szBandName, sizeof(szBandName), "%d", iBand+1 );
                xmlNewChild( psKeys, NULL, BAD_CAST "Key", 
                             BAD_CAST szBandName );
            }
        }
    }        
        
/* -------------------------------------------------------------------- */
/*      SupportedCRS                                                    */
/* -------------------------------------------------------------------- */
    {
        char *owned_value;
        
        if( (owned_value = 
             msOWSGetProjURN( &(layer->projection), &(layer->metadata), 
                              "CO", MS_FALSE)) != NULL ) {
            /* ok */
        } else if((owned_value = 
                   msOWSGetProjURN( &(layer->map->projection), 
                                    &(layer->map->web.metadata), 
                                    "CO", MS_FALSE)) != NULL ) {
            /* ok */
        } else 
            msDebug( "mapwcs.c: missing required information, no SRSs defined.\n");
        
        if( owned_value != NULL && strlen(owned_value) > 0 ) 
            msLibXml2GenerateList( psCD, NULL, "SupportedCRS", 
                                    owned_value, ' ' );

        msFree( owned_value );
    }

/* -------------------------------------------------------------------- */
/*      SupportedFormats                                                */
/* -------------------------------------------------------------------- */
    {
        char *format_list;
        
        format_list = msWCSGetFormatsList11( layer->map, layer );
        
        if (strlen(format_list) > 0 )
            msLibXml2GenerateList( psCD, NULL, "SupportedFormat", 
                                    format_list, ',' );
        
        msFree( format_list );
    }
    
    return MS_SUCCESS;
}

/************************************************************************/
/*                      msWCSDescribeCoverage11()                       */
/************************************************************************/

int msWCSDescribeCoverage11(mapObj *map, wcsParamsObj *params, owsRequestObj *ows_request)
{
    xmlDocPtr psDoc = NULL;       /* document pointer */
    xmlNodePtr psRootNode;
    xmlNsPtr psOwsNs, psXLinkNs;
    char *schemaLocation = NULL;
    char *xsi_schemaLocation = NULL;
    const char *encoding;

    int i,j;

    encoding = msOWSLookupMetadata(&(map->web.metadata), "CO", "encoding");

/* -------------------------------------------------------------------- */
/*      We will actually get the coverages list as a single item in     */
/*      a string list with that item having the comma delimited         */
/*      coverage names.  Split it up now, and assign back in place      */
/*      of the old coverages list.                                      */
/* -------------------------------------------------------------------- */
    if( CSLCount(params->coverages) == 1 )
    {
        char **old_coverages = params->coverages;
        params->coverages = CSLTokenizeStringComplex( old_coverages[0], ",",
                                                      FALSE, FALSE );
        CSLDestroy( old_coverages );
    }

/* -------------------------------------------------------------------- */
/*      Validate that the requested coverages exist as named layers.    */
/* -------------------------------------------------------------------- */
    if(params->coverages) { /* use the list */
        for( j = 0; params->coverages[j]; j++ ) {
            i = msGetLayerIndex(map, params->coverages[j]);
            if ( (i == -1) || 
                 (!msIntegerInArray(GET_LAYER(map, i)->index, ows_request->enabled_layers, ows_request->numlayers)) )
            {
                msSetError( MS_WCSERR,
                            "COVERAGE %s cannot be opened / does not exist",
                            "msWCSDescribeCoverage()", params->coverages[j]);
                return msWCSException11(map, "coverage", "CoverageNotDefined", params->version);
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Create document.                                                */
/* -------------------------------------------------------------------- */
    psDoc = xmlNewDoc(BAD_CAST "1.0");

    psRootNode = xmlNewNode(NULL, BAD_CAST "CoverageDescriptions");

    xmlDocSetRootElement(psDoc, psRootNode);

/* -------------------------------------------------------------------- */
/*      Name spaces                                                     */
/* -------------------------------------------------------------------- */
    xmlSetNs(psRootNode, xmlNewNs(psRootNode, BAD_CAST "http://www.opengis.net/wcs/1.1", NULL));
    psOwsNs = xmlNewNs(psRootNode, BAD_CAST MS_OWSCOMMON_OWS_110_NAMESPACE_URI, BAD_CAST MS_OWSCOMMON_OWS_NAMESPACE_PREFIX);
    psXLinkNs = xmlNewNs(psRootNode, BAD_CAST MS_OWSCOMMON_W3C_XLINK_NAMESPACE_URI, BAD_CAST MS_OWSCOMMON_W3C_XLINK_NAMESPACE_PREFIX);
    xmlNewNs(psRootNode, BAD_CAST MS_OWSCOMMON_W3C_XSI_NAMESPACE_URI, BAD_CAST MS_OWSCOMMON_W3C_XSI_NAMESPACE_PREFIX);
    xmlNewNs(psRootNode, BAD_CAST MS_OWSCOMMON_OGC_NAMESPACE_URI, BAD_CAST MS_OWSCOMMON_OGC_NAMESPACE_PREFIX );

    schemaLocation = msEncodeHTMLEntities( msOWSGetSchemasLocation(map) );
    xsi_schemaLocation = msStrdup("http://www.opengis.net/wcs/1.1");
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, " ");
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, schemaLocation);
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, "/wcs/1.1/wcsDescribeCoverage.xsd ");
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, MS_OWSCOMMON_OWS_110_NAMESPACE_URI);
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, " ");
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, schemaLocation);
    xsi_schemaLocation = msStringConcatenate(xsi_schemaLocation, "/ows/1.1.0/owsAll.xsd");
    xmlNewNsProp(psRootNode, NULL, BAD_CAST "xsi:schemaLocation", BAD_CAST xsi_schemaLocation);

/* -------------------------------------------------------------------- */
/*      Generate a CoverageDescription for each requested coverage.     */
/* -------------------------------------------------------------------- */

    if(params->coverages) { /* use the list */
        for( j = 0; params->coverages[j]; j++ ) {
            i = msGetLayerIndex(map, params->coverages[j]);
            msWCSDescribeCoverage_CoverageDescription11((GET_LAYER(map, i)), 
                                                        params, psRootNode,
                                                        psOwsNs );
        }
    } else { /* return all layers */
        for(i=0; i<map->numlayers; i++) {

            if (!msIntegerInArray(GET_LAYER(map, i)->index, ows_request->enabled_layers, ows_request->numlayers))
                continue;

            msWCSDescribeCoverage_CoverageDescription11((GET_LAYER(map, i)), 
                                                        params, psRootNode,
                                                        psOwsNs );
        }
    }
  
/* -------------------------------------------------------------------- */
/*      Write out the document.                                         */
/* -------------------------------------------------------------------- */
    {
        xmlChar *buffer = NULL;
        int size = 0;
        msIOContext *context = NULL;

        if( msIO_needBinaryStdout() == MS_FAILURE )
            return MS_FAILURE;
     
        if (encoding)
            msIO_printf("Content-type: text/xml; charset=%s%c%c", encoding,10,10);
        else
            msIO_printf("Content-type: text/xml%c%c",10,10);
    
        context = msIO_getHandler(stdout);

        xmlDocDumpFormatMemoryEnc(psDoc, &buffer, &size, (encoding ? encoding : "ISO-8859-1"), 1);
        msIO_contextWrite(context, buffer, size);
        xmlFree(buffer);
    }
        
/* -------------------------------------------------------------------- */
/*      Cleanup                                                         */
/* -------------------------------------------------------------------- */
    xmlFreeDoc(psDoc);
    xmlCleanupParser();

    return MS_SUCCESS;
}

#endif /* defined(USE_WCS_SVR) && defined(USE_LIBXML2) */

/************************************************************************/
/*                      msWCSGetCoverageBands11()                       */
/*                                                                      */
/*      We expect input to be of the form:                              */
/*      RangeSubset=raster:interpolation[bands[1]].                     */
/*                                                                      */
/*      RangeSet=raster:[bands[1,2]]                                    */
/*       or                                                             */
/*      RangeSet=raster:bilinear                                        */
/*                                                                      */
/*      This function tries to return a bandlist if found, and will     */
/*      also push an INTERPOLATION keyword into the parameters list     */
/*      if found in the RangeSubset.                                    */
/************************************************************************/

#if defined(USE_WCS_SVR)
int msWCSGetCoverageBands11( mapObj *map, cgiRequestObj *request, 
                             wcsParamsObj *params, layerObj *lp,
                             char **p_bandlist )

{
    char *rangesubset, *field_id;
    const char *axis_id, *value;
    int i;

/* -------------------------------------------------------------------- */
/*      Fetch the RangeSubset from the parameters, skip building a      */
/*      bands list if not found.                                        */
/* -------------------------------------------------------------------- */
    value = msWCSGetRequestParameter(request, "RangeSubset");
    if( value == NULL )
        return MS_SUCCESS;

    rangesubset = msStrdup(value);

/* -------------------------------------------------------------------- */
/*      What is the <Field identifier=...> (rangeset_name)?             */
/* -------------------------------------------------------------------- */
    value = msOWSLookupMetadata( &(lp->metadata), "CO", "rangeset_name" );
    if( value == NULL )
        value = "raster";
    field_id = msStrdup(value);

/* -------------------------------------------------------------------- */
/*      What is the <Axis identifier=...> (bands_name)?                 */
/* -------------------------------------------------------------------- */
    axis_id = msOWSLookupMetadata( &(lp->metadata), "CO", "bands_name" );
    if( axis_id == NULL )
        axis_id = "bands";

/* -------------------------------------------------------------------- */
/*      Parse out the field identifier from the request and verify.     */
/* -------------------------------------------------------------------- */
    value = rangesubset + strlen(field_id);

    if( strcasecmp(rangesubset,field_id) == 0 )
        return MS_SUCCESS; /* we only got field ... default options */

    if( strlen(rangesubset) <= strlen(field_id)+1 
        || strncasecmp(rangesubset,field_id,strlen(field_id)) != 0 
        || (*value != '[' && *value != ':') )
    {
        msSetError( MS_WCSERR, 
                    "RangeSubset field name malformed, expected '%s', got RangeSubset=%s",
                    "msWCSGetCoverageBands11()", 
                    field_id, rangesubset );
        return msWCSException11(map, "mapserv", "NoApplicableCode", params->version);
    }

    free( field_id );
    field_id = NULL;
    
/* -------------------------------------------------------------------- */
/*      Parse out the interpolation, if found.                          */
/* -------------------------------------------------------------------- */
    if( *value == ':' )
    {
        assert( params->interpolation == NULL );
        params->interpolation = msStrdup(value+1);
        for( i = 0; params->interpolation[i] != '\0'; i++ )
        {
            if( params->interpolation[i] == '[' )
            {
                params->interpolation[i] = '\0';
                break;
            }
        }

        value += strlen(params->interpolation) + 1;
    }

/* -------------------------------------------------------------------- */
/*      Parse out the axis name, and verify.                            */
/* -------------------------------------------------------------------- */
    if( *value != '[' )
        return MS_SUCCESS;

    value++;

    if( strlen(value) <= strlen(axis_id)+1
        || strncasecmp(value,axis_id,strlen(axis_id)) != 0
        || value[strlen(axis_id)] != '[' )
    {
        msSetError( MS_WCSERR, 
                    "RangeSubset axis name malformed, expected '%s', got RangeSubset=%s",
                    "msWCSGetCoverageBands11()", 
                    axis_id, rangesubset );
        return msWCSException11(map, "mapserv", "NoApplicableCode", params->version);
    }

/* -------------------------------------------------------------------- */
/*      Parse the band list.  Basically assuming the band list is       */
/*      everything from here to a close ';'.                            */
/* -------------------------------------------------------------------- */
    value += strlen(axis_id) + 1;

    *p_bandlist = msStrdup(value);

    for( i = 0; (*p_bandlist)[i] != '\0'; i++ )
    {
        if( (*p_bandlist)[i] == '[' )
        {
            (*p_bandlist)[i] = '\0';
            break;
        }
    }

    return MS_SUCCESS;
}    
#endif

/************************************************************************/
/*                       msWCSReturnCoverage11()                        */
/*                                                                      */
/*      Return a render image as a coverage to the caller with WCS      */
/*      1.1 "mime" wrapping.                                            */
/************************************************************************/

#if defined(USE_WCS_SVR)
int  msWCSReturnCoverage11( wcsParamsObj *params, mapObj *map, 
                            imageObj *image )
{
    int status, i;
    char *filename = NULL;
    const char *encoding;
    const char *fo_filename;

    encoding = msOWSLookupMetadata(&(map->web.metadata), "CO", "encoding");

    fo_filename = msGetOutputFormatOption( image->format, "FILENAME", NULL );
        
/* -------------------------------------------------------------------- */
/*      Fetch the driver we will be using and check if it supports      */
/*      VSIL IO.                                                        */
/* -------------------------------------------------------------------- */
#ifdef GDAL_DCAP_VIRTUALIO
    if( EQUALN(image->format->driver,"GDAL/",5) )
    {
        GDALDriverH hDriver;
        const char *pszExtension = image->format->extension;

        msAcquireLock( TLOCK_GDAL );
        hDriver = GDALGetDriverByName( image->format->driver+5 );
        if( hDriver == NULL )
        {
            msReleaseLock( TLOCK_GDAL );
            msSetError( MS_MISCERR, 
                        "Failed to find %s driver.",
                        "msWCSReturnCoverage11()", 
                        image->format->driver+5 );
            return msWCSException11(map, "mapserv", "NoApplicableCode", 
                                    params->version);
        }
        
        if( pszExtension == NULL )
            pszExtension = "img.tmp";

        if( GDALGetMetadataItem( hDriver, GDAL_DCAP_VIRTUALIO, NULL ) 
            != NULL )
        {
            if( fo_filename )
                filename = msStrdup(CPLFormFilename("/vsimem/wcsout",
                                                    fo_filename,NULL));
            else
                filename = msStrdup(CPLFormFilename("/vsimem/wcsout", 
                                                    "out", pszExtension ));

/*            CleanVSIDir( "/vsimem/wcsout" ); */
            
            msReleaseLock( TLOCK_GDAL );
            status = msSaveImage(map, image, filename);
            if( status != MS_SUCCESS )
            {
                msSetError(MS_MISCERR, "msSaveImage() failed", 
                           "msWCSReturnCoverage11()");
                return msWCSException11(map, "mapserv", "NoApplicableCode", 
                                        params->version);
            }
        }
        msReleaseLock( TLOCK_GDAL );
    }
#endif

/* -------------------------------------------------------------------- */
/*      Output stock header.                                            */
/* -------------------------------------------------------------------- */
    if (encoding)
        msIO_fprintf( 
            stdout, 
            "Content-Type: multipart/mixed; boundary=wcs%c%c"
            "--wcs\n"
            "Content-Type: text/xml; charset=%s\n"
            "Content-ID: wcs.xml%c%c"
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<Coverages\n"
            "     xmlns=\"http://www.opengis.net/wcs/1.1\"\n"
            "     xmlns:ows=\"http://www.opengis.net/ows\"\n"
            "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
            "     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
            "     xsi:schemaLocation=\"http://www.opengis.net/ows/1.1 ../owsCoverages.xsd\">\n"
            "  <Coverage>\n",
            10, 10,
            encoding,
            10, 10 );
    else
        msIO_fprintf( 
            stdout, 
            "Content-Type: multipart/mixed; boundary=wcs%c%c"
            "--wcs\n"
            "Content-Type: text/xml\n"
            "Content-ID: wcs.xml%c%c"
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<Coverages\n"
            "     xmlns=\"http://www.opengis.net/wcs/1.1\"\n"
            "     xmlns:ows=\"http://www.opengis.net/ows\"\n"
            "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
            "     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
            "     xsi:schemaLocation=\"http://www.opengis.net/ows/1.1 ../owsCoverages.xsd\">\n"
            "  <Coverage>\n",
            10, 10,
            10, 10 );

/* -------------------------------------------------------------------- */
/*      If we weren't able to write data under /vsimem, then we just    */
/*      output a single "stock" filename.                               */
/* -------------------------------------------------------------------- */
    if( filename == NULL )
    {
        msIO_fprintf( 
            stdout,
            "    <Reference xlink:href=\"cid:coverage/wcs.%s\"/>\n"
            "  </Coverage>\n"
            "</Coverages>\n"
            "--wcs\n"
            "Content-Type: %s\n"
            "Content-Description: coverage data\n"
            "Content-Transfer-Encoding: binary\n"
            "Content-ID: coverage/wcs.%s\n"
            "Content-Disposition: INLINE%c%c",
            MS_IMAGE_EXTENSION(map->outputformat),
            MS_IMAGE_MIME_TYPE(map->outputformat),
            MS_IMAGE_EXTENSION(map->outputformat),
            10, 10 );

        status = msSaveImage(map, image, NULL);
        if( status != MS_SUCCESS )
        {
            msSetError( MS_MISCERR, "msSaveImage() failed", "msWCSReturnCoverage11()");
            return msWCSException11(map, "mapserv", "NoApplicableCode", params->version);
        }

        msIO_fprintf( stdout, "\n--wcs--%c%c", 10, 10 );
        return MS_SUCCESS;
    }

/* -------------------------------------------------------------------- */
/*      When potentially listing multiple files, we take great care     */
/*      to identify the "primary" file and list it first.  In fact      */
/*      it is the only file listed in the coverages document.           */
/* -------------------------------------------------------------------- */
#ifdef GDAL_DCAP_VIRTUALIO
    {
        char **all_files = CPLReadDir( "/vsimem/wcsout" );
        int count = CSLCount(all_files);

        if( msIO_needBinaryStdout() == MS_FAILURE )
            return MS_FAILURE;

        msAcquireLock( TLOCK_GDAL );
        for( i = count-1; i >= 0; i-- )
        {
            const char *this_file = all_files[i];

            if( EQUAL(this_file,".") || EQUAL(this_file,"..") )
            {
                all_files = CSLRemoveStrings( all_files, i, 1, NULL );
                continue;
            }

            if( i > 0 && EQUAL(this_file,CPLGetFilename(filename)) )
            {
                all_files = CSLRemoveStrings( all_files, i, 1, NULL );
                all_files = CSLInsertString(all_files,0,CPLGetFilename(filename));
                i++;
            }
        }
        
        msIO_fprintf( 
            stdout,
            "    <Reference xlink:href=\"cid:coverage/%s\"/>\n"
            "  </Coverage>\n"
            "</Coverages>\n",
            CPLGetFilename(filename) );

/* -------------------------------------------------------------------- */
/*      Dump all the files in the memory directory as mime sections.    */
/* -------------------------------------------------------------------- */
        count = CSLCount(all_files);

        for( i = 0; i < count; i++ )
        {
            const char *mimetype = NULL;
            FILE *fp; 
            unsigned char block[4000];
            int bytes_read;

            if( i == 0 )
                mimetype = MS_IMAGE_MIME_TYPE(map->outputformat);
            
            if( mimetype == NULL )
                mimetype = "application/octet-stream";

            msIO_fprintf( 
                stdout, 
                "--wcs\n"
                "Content-Type: %s\n"
                "Content-Description: coverage data\n"
                "Content-Transfer-Encoding: binary\n"
                "Content-ID: coverage/%s\n"
                "Content-Disposition: INLINE%c%c",
                mimetype, 
                all_files[i], 
                10, 10 );

            fp = VSIFOpenL( 
                CPLFormFilename("/vsimem/wcsout", all_files[i], NULL),
                "rb" );
            if( fp == NULL )
            {
                msReleaseLock( TLOCK_GDAL );
                msSetError( MS_MISCERR, 
                            "Failed to open %s for streaming to stdout.",
                            "msWCSReturnCoverage11()", all_files[i] );
                return MS_FAILURE;
            }
            
            while( (bytes_read = VSIFReadL(block, 1, sizeof(block), fp)) > 0 )
                msIO_fwrite( block, 1, bytes_read, stdout );

            VSIFCloseL( fp );

            VSIUnlink( all_files[i] );
        }

        CSLDestroy( all_files );
        msReleaseLock( TLOCK_GDAL );

        msIO_fprintf( stdout, "\n--wcs--%c%c", 10, 10 );
        return MS_SUCCESS;
    }
#else
    return MS_SUCCESS;
#endif /* def GDAL_DCAP_VIRTUALIO */
}
#endif /* defined(USE_WCS_SVR) && defined(USE_LIBXML2) */

/************************************************************************/
/* ==================================================================== */
/*	If we don't have libxml2 but WCS SVR was selected, then         */
/*      report WCS 1.1 requests as unsupported.                         */
/* ==================================================================== */
/************************************************************************/

#if defined(USE_WCS_SVR) && !defined(USE_LIBXML2)

#include "mapwcs.h"

/* ==================================================================== */

int msWCSDescribeCoverage11(mapObj *map, wcsParamsObj *params)

{
    msSetError( MS_WCSERR,
                "WCS 1.1 request made, but mapserver requires libxml2 for WCS 1.1 services and this is not configured.",
                "msWCSDescribeCoverage11()", "NoApplicableCode" );
    return msWCSException11(map, "mapserv", "NoApplicableCode", params->version);
}

/* ==================================================================== */

int msWCSGetCapabilities11(mapObj *map, wcsParamsObj *params, 
                                  cgiRequestObj *req)

{
    msSetError( MS_WCSERR,
                "WCS 1.1 request made, but mapserver requires libxml2 for WCS 1.1 services and this is not configured.",
                "msWCSGetCapabilities11()", "NoApplicableCode" );

    return msWCSException11(map, "mapserv", "NoApplicableCode", params->version);
}

int msWCSException11(mapObj *map, const char *locator, const char *exceptionCode, const char *version) {
    /* fallback to reporting using 1.0 style exceptions. */
    return msWCSException( map, locator, exceptionCode, "1.0.0" );
}

#endif /* defined(USE_WCS_SVR) && !defined(USE_LIBXML2) */
