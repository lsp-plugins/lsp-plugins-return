/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_META_RETURN_H_
#define PRIVATE_META_RETURN_H_

#include <lsp-plug.in/plug-fw/meta/types.h>
#include <lsp-plug.in/plug-fw/const.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Plugin metadata
    namespace meta
    {
        typedef struct Return
        {
            static constexpr float  RETURN_GAIN_MIN     = GAIN_AMP_M_INF_DB;
            static constexpr float  RETURN_GAIN_MAX     = GAIN_AMP_P_60_DB;
            static constexpr float  RETURN_GAIN_DFL     = GAIN_AMP_0_DB;
            static constexpr float  RETURN_GAIN_STEP    = GAIN_AMP_S_0_5_DB;
        } Return;

        // Plugin type metadata
        extern const plugin_t return_mono;
        extern const plugin_t return_stereo;

    } /* namespace meta */
} /* namespace lsp */

#endif /* PRIVATE_META_RETURN_H_ */
