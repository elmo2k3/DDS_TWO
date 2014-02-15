#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <stdint.h>
#include "page_singletone.h"
#include "page_graph.h"

extern struct _settings settings;

enum UNIT_FORMAT
{
    UNIT_RAW,
    UNIT_DBUV,
    UNIT_DBMW
};

struct _settings
{
    uint8_t output_active;
    enum UNIT_FORMAT unit_format;
    struct page_graph_settings graph_settings;
    struct page_singletone_settings singletone_settings;
};

void settings_init(void);

#endif
