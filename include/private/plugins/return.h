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

#ifndef PRIVATE_PLUGINS_RETURN_H_
#define PRIVATE_PLUGINS_RETURN_H_

#include <lsp-plug.in/dsp-units/util/Delay.h>
#include <lsp-plug.in/dsp-units/ctl/Bypass.h>
#include <lsp-plug.in/plug-fw/plug.h>
#include <private/meta/return.h>

namespace lsp
{
    namespace plugins
    {
        /**
         * Base class for the latency compensation delay
         */
        class Return: public plug::Module
        {
            private:
                enum mode_t
                {
                    MODE_ADD,
                    MODE_MUL,
                    MODE_REPLACE
                };

                typedef struct channel_t
                {
                    dspu::Bypass        sBypass;        // Bypass

                    plug::IPort        *pIn;            // Input port
                    plug::IPort        *pOut;           // Output port
                    plug::IPort        *pReturn;        // Return port

                    plug::IPort        *pInMeter;       // Input level meter
                    plug::IPort        *pOutMeter;      // Output level meter
                    plug::IPort        *pReturnMeter;   // Return level meter
                } channel_t;

            protected:
                size_t              nChannels;          // Number of channels
                channel_t          *vChannels;          // Channels
                float               fInGain;            // Input gain
                float               fOutGain;           // Output gain
                float               fReturnGain;        // Return gain
                mode_t              enMode;             // Return mode

                plug::IPort        *pBypass;            // Bypass port
                plug::IPort        *pInGain;            // Input gain
                plug::IPort        *pOutGain;           // Output gain
                plug::IPort        *pMode;              // Return mode
                plug::IPort        *pReturnGain;        // Return gain

            protected:
                void                do_destroy();
                static mode_t       decode_mode(ssize_t mode);

            public:
                explicit Return(const meta::plugin_t *meta);
                Return (const Return &) = delete;
                Return (Return &&) = delete;
                virtual ~Return() override;

                Return & operator = (const Return &) = delete;
                Return & operator = (Return &&) = delete;

                virtual void        init(plug::IWrapper *wrapper, plug::IPort **ports) override;
                virtual void        destroy() override;

            public:
                virtual void        update_sample_rate(long sr) override;
                virtual void        update_settings() override;
                virtual void        process(size_t samples) override;
                virtual void        dump(dspu::IStateDumper *v) const override;
        };

    } /* namespace plugins */
} /* namespace lsp */


#endif /* PRIVATE_PLUGINS_RETURN_H_ */

