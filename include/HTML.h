#include <Arduino.h>

#if defined(TestPage)
#include "HTML/testpage2.htm"   //  HTML
#else
const char *test_html PROGMEM = "";
#endif

// #include "HTML/Parts/Body_TestPage.htm"   //  HTML

#if (TestPage == 4) || (HomePage == 4)
#include "HTML/Parts/Content_I2C.htm"
#endif
#if (TestPage == 1) || (HomePage == 1)
#include "HTML/Parts/Content_Poem.htm"
#endif
#if (TestPage == 2) || (HomePage == 2)
#include "HTML/Parts/Content_SiniLink.htm"
#endif
#if (TestPage == 5) || (HomePage == 5)
#include "HTML/Parts/Content_Sonoff.htm"
#endif

// extern const char Content_I2C[];
// extern const char Content_Poem[];
// extern const char Content_SiniLink[];
// extern const char Content_Sonoff[];
