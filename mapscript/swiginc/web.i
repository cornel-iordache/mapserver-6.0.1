/* ===========================================================================
   $Id: web.i 6373 2007-07-24 02:37:18Z dmorissette $
 
   Project:  MapServer
   Purpose:  SWIG interface file for mapscript webObj extensions
   Author:   Steve Lime 
             Umberto Nicoletti unicoletti@prometeo.it
             
   ===========================================================================
   Copyright (c) 1996-2001 Regents of the University of Minnesota.
   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:
 
   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.
 
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
   ===========================================================================
*/


%include "../../mapserver.h"

/* Constructor and destructor for webObj
   http://mapserver.gis.umn.edu/bugs/show_bug.cgi?id=579 */

%extend webObj 
{
    
    webObj() 
    {
        webObj *web;
        web = (webObj *) malloc(sizeof(webObj));
        initWeb(web);
        return web;
    }

    ~webObj() 
    {
        if (!self) return;
	freeWeb(self);
        free(self);
    }

    int updateFromString(char *snippet)
    {
        return msUpdateWebFromString(self, snippet, MS_FALSE);
    }
}


