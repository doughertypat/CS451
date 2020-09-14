/**********************************************************
 * CS451 - HW1
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 4 September 2020
 *********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <regex.h>

int main()
{
    const char *wgetCmd = "wget -q http://www.senate.gov/general/contact_information/senators_cfm.cfm -O senators_cfm.cfm";
    system(wgetCmd);

    const char *grepCmd = "grep -Po \"(href)([^#\\n\\r<>]*)?\" senators_cfm.cfm | grep -Po \"((https?:\\/\\/)|(www\\.))([\\w\\d\\-_\\.]+)\\/?\\??([^#\\n\\r\\\"<>]*)?#?([^\\n\\r\\\"<>]*)\"";
    system(grepCmd);
    
    const char *unlinkCmd = "unlink senators_cfm.cfm";
    system(unlinkCmd);

    return 0;
}
