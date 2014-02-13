#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <stdint.h>
#include "page_singletone.h"
#include "page_graph.h"

extern struct _settings settings;

struct _settings
{
    uint8_t output_active;
    struct page_graph_settings graph_settings;
    struct page_singletone_settings singletone_settings;
};

#endif
