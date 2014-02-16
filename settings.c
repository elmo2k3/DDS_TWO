#include "settings.h"

struct _settings settings;

void settings_init()
{
    settings.output_active = 0;
    //settings.unit_format = UNIT_RAW;
    page_graph_settings_init();
    page_singletone_settings_init();
}

