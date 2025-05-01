/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins-return
 * Created on: 26 авг 2024 г.
 *
 * lsp-plugins-return is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins-return is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins-return. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/plug-fw/meta/ports.h>
#include <lsp-plug.in/shared/meta/developers.h>
#include <private/meta/return.h>

#define LSP_PLUGINS_RETURN_VERSION_MAJOR       1
#define LSP_PLUGINS_RETURN_VERSION_MINOR       0
#define LSP_PLUGINS_RETURN_VERSION_MICRO       4

#define LSP_PLUGINS_RETURN_VERSION  \
    LSP_MODULE_VERSION( \
        LSP_PLUGINS_RETURN_VERSION_MAJOR, \
        LSP_PLUGINS_RETURN_VERSION_MINOR, \
        LSP_PLUGINS_RETURN_VERSION_MICRO  \
    )

namespace lsp
{
    namespace meta
    {
        //-------------------------------------------------------------------------
        // Plugin metadata

        static const port_item_t return_mix_mode[] =
        {
            { "Add",                        "return.mode.add" },
            { "Multiply",                   "return.mode.mul" },
            { "Replace",                    "return.mode.rep" },
            { NULL, NULL }
        };

        // NOTE: Port identifiers should not be longer than 7 characters as it will overflow VST2 parameter name buffers
        static const port_t return_mono_ports[] =
        {
            PORTS_MONO_PLUGIN,

            BYPASS,
            IN_GAIN,
            OUT_GAIN,
            COMBO("mode", "Return mode", 0, return_mix_mode),
            LOG_CONTROL("g_retn", "Return gain", "Return gain", U_GAIN_AMP, Return::RETURN_GAIN),
            RETURN_NAME("return", "Audio return connection point name"),
            AUDIO_RETURN("rin", "Audio return input", 0, "return"),

            METER_GAIN("ilm", "Input level meter", GAIN_AMP_P_24_DB),
            METER_GAIN("rlm", "Return level meter", GAIN_AMP_P_24_DB),
            METER_GAIN("olm", "Output level meter", GAIN_AMP_P_24_DB),

            PORTS_END
        };

        // NOTE: Port identifiers should not be longer than 7 characters as it will overflow VST2 parameter name buffers
        static const port_t return_stereo_ports[] =
        {
            PORTS_STEREO_PLUGIN,

            BYPASS,
            IN_GAIN,
            OUT_GAIN,
            COMBO("mode", "Return mode", 0, return_mix_mode),
            LOG_CONTROL("g_retn", "Return gain", "Return gain", U_GAIN_AMP, Return::RETURN_GAIN),
            RETURN_NAME("return", "Audio return  connection point name"),
            AUDIO_RETURN("rin_l", "Audio return input left", 0, "return"),
            AUDIO_RETURN("rin_r", "Audio return input right", 1, "return"),

            METER_GAIN("ilm_l", "Input level meter Left", GAIN_AMP_P_24_DB),
            METER_GAIN("rlm_l", "Return level meter Left", GAIN_AMP_P_24_DB),
            METER_GAIN("olm_l", "Output level meter Left", GAIN_AMP_P_24_DB),
            METER_GAIN("ilm_r", "Input level meter Right", GAIN_AMP_P_24_DB),
            METER_GAIN("rlm_r", "Return level meter Right", GAIN_AMP_P_24_DB),
            METER_GAIN("olm_r", "Output level meter Right", GAIN_AMP_P_24_DB),

            PORTS_END
        };

        static const int plugin_classes[]       = { C_UTILITY, -1 };
        static const int clap_features_mono[]   = { CF_AUDIO_EFFECT, CF_UTILITY, CF_MONO, -1 };
        static const int clap_features_stereo[] = { CF_AUDIO_EFFECT, CF_UTILITY, CF_STEREO, -1 };

        const meta::bundle_t return_bundle =
        {
            "return",
            "Return",
            B_UTILITIES,
            "x336_XxHp2o",
            "This plugin allows to perform audio return using the shared memory"
        };

        const plugin_t return_mono =
        {
            "Return Mono",
            "Return Mono",
            "Return Mono",
            "R1M",
            &developers::v_sadovnikov,
            "return_mono",
            {
                LSP_LV2_URI("return_mono"),
                LSP_LV2UI_URI("return_mono"),
                "r01m",
                LSP_VST3_UID("r01m    r01m"),
                LSP_VST3UI_UID("r01s    r01m"),
                0,
                NULL,
                LSP_CLAP_URI("return_mono"),
                LSP_GST_UID("return_mono"),
            },
            LSP_PLUGINS_RETURN_VERSION,
            plugin_classes,
            clap_features_mono,
            E_DUMP_STATE | E_SHM_TRACKING,
            return_mono_ports,
            "util/return.xml",
            NULL,
            mono_plugin_port_groups,
            &return_bundle
        };

        const plugin_t return_stereo =
        {
            "Return Stereo",
            "Return Stereo",
            "Return Stereo",
            "R1S",
            &developers::v_sadovnikov,
            "return_stereo",
            {
                LSP_LV2_URI("return_stereo"),
                LSP_LV2UI_URI("return_stereo"),
                "r01s",
                LSP_VST3_UID("r01s    r01s"),
                LSP_VST3UI_UID("r01s    r01s"),
                0,
                NULL,
                LSP_CLAP_URI("return_stereo"),
                LSP_GST_UID("return_stereo"),
            },
            LSP_PLUGINS_RETURN_VERSION,
            plugin_classes,
            clap_features_stereo,
            E_DUMP_STATE | E_SHM_TRACKING,
            return_stereo_ports,
            "util/return.xml",
            NULL,
            stereo_plugin_port_groups,
            &return_bundle
        };
    } /* namespace meta */
} /* namespace lsp */



