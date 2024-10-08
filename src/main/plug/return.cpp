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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/dsp-units/units.h>
#include <lsp-plug.in/plug-fw/core/AudioBuffer.h>
#include <lsp-plug.in/plug-fw/meta/func.h>
#include <lsp-plug.in/shared/debug.h>

#include <private/plugins/return.h>

namespace lsp
{
    namespace plugins
    {
        //---------------------------------------------------------------------
        // Plugin factory
        static const meta::plugin_t *plugins[] =
        {
            &meta::return_mono,
            &meta::return_stereo
        };

        static plug::Module *plugin_factory(const meta::plugin_t *meta)
        {
            return new Return(meta);
        }

        static plug::Factory factory(plugin_factory, plugins, 2);

        //---------------------------------------------------------------------
        // Implementation
        Return::Return(const meta::plugin_t *meta):
            Module(meta)
        {
            // Compute the number of audio channels by the number of inputs
            nChannels       = 0;

            for (const meta::port_t *p = meta->ports; p->id != NULL; ++p)
                if (meta::is_audio_in_port(p))
                    ++nChannels;

            vChannels       = NULL;
            fInGain         = GAIN_AMP_M_INF_DB;
            fOutGain        = GAIN_AMP_M_INF_DB;
            fReturnGain     = GAIN_AMP_M_INF_DB;
            enMode          = MODE_ADD;

            pBypass         = NULL;
            pInGain         = NULL;
            pOutGain        = NULL;
            pMode           = NULL;
            pReturnGain     = NULL;
        }

        Return::~Return()
        {
            do_destroy();
        }

        void Return::init(plug::IWrapper *wrapper, plug::IPort **ports)
        {
            // Call parent class for initialization
            Module::init(wrapper, ports);

            size_t to_alloc     = sizeof(channel_t) * nChannels;
            vChannels           = static_cast<channel_t *>(malloc(to_alloc));
            if (vChannels == NULL)
                return;

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];

                c->sBypass.construct();

                c->pIn              = NULL;
                c->pOut             = NULL;
                c->pReturn          = NULL;

                c->pInMeter         = NULL;
                c->pOutMeter        = NULL;
                c->pReturnMeter     = NULL;
            }

            size_t port_id      = 0;

            // Bind inputs and outpus
            lsp_trace("Binding inputs and outputs");
            for (size_t i=0; i<nChannels; ++i)
                BIND_PORT(vChannels[i].pIn);
            for (size_t i=0; i<nChannels; ++i)
                BIND_PORT(vChannels[i].pOut);

            lsp_trace("Binding common ports");
            BIND_PORT(pBypass);
            BIND_PORT(pInGain);
            BIND_PORT(pOutGain);
            BIND_PORT(pMode);
            BIND_PORT(pReturnGain);

            lsp_trace("Binding return ports");
            SKIP_PORT("Return name");
            for (size_t i=0; i<nChannels; ++i)
                BIND_PORT(vChannels[i].pReturn);

            lsp_trace("Binding meters");
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];

                BIND_PORT(c->pInMeter);
                BIND_PORT(c->pReturnMeter);
                BIND_PORT(c->pOutMeter);
            }
        }

        void Return::destroy()
        {
            Module::destroy();
            do_destroy();
        }

        void Return::do_destroy()
        {
            if (vChannels != NULL)
            {
                for (size_t i=0; i<nChannels; ++i)
                {
                    channel_t *c        = &vChannels[i];
                    c->sBypass.destroy();
                }

                free(vChannels);
                vChannels       = NULL;
            }
        }

        Return::mode_t Return::decode_mode(ssize_t mode)
        {
            switch (mode)
            {
                case 0: return MODE_ADD;
                case 1: return MODE_MUL;
                default: break;
            }
            return MODE_REPLACE;
        }

        void Return::update_sample_rate(long sr)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];
                c->sBypass.init(sr);
            }
        }

        void Return::update_settings()
        {
            const bool bypass   = pBypass->value() >= 0.5f;

            fInGain             = pInGain->value();
            fOutGain            = pOutGain->value();
            fReturnGain         = pReturnGain->value();
            enMode              = decode_mode(pMode->value());

            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];
                c->sBypass.set_bypass(bypass);
            }
        }

        void Return::process(size_t samples)
        {
            for (size_t i=0; i<nChannels; ++i)
            {
                channel_t *c        = &vChannels[i];
                const float *in     = c->pIn->buffer<float>();
                float *out          = c->pOut->buffer<float>();

                core::AudioBuffer *retn_buf = c->pReturn->buffer<core::AudioBuffer>();
                float *retn         = ((retn_buf != NULL) && (retn_buf->active())) ? retn_buf->buffer() : NULL;

                const float ilm     = dsp::abs_max(in, samples) * fInGain;
                float rlm           = 0.0f;
                float olm           = 0.0f;

                if (retn != NULL)
                {
                    // Process return signal part
                    c->sBypass.process_wet(out, NULL, retn, fReturnGain, samples);
                    rlm                 = dsp::abs_max(out, samples);

                    // Mix return with input
                    switch (enMode)
                    {
                        case MODE_ADD:
                            dsp::mix2(out, in, fOutGain, fInGain * fOutGain, samples);
                            break;
                        case MODE_MUL:
                            dsp::fmmul_k3(out, in, fInGain, samples);
                            dsp::mul_k2(out, fOutGain, samples);
                            break;
                        case MODE_REPLACE:
                        default:
                            dsp::mul_k2(out, fOutGain, samples);
                            break;
                    }
                    olm                 = dsp::abs_max(out, samples);
                }
                else
                {
                    if (enMode == MODE_ADD)
                        dsp::mul_k3(out, in, fInGain * fOutGain, samples);
                    else
                        dsp::fill_zero(out, samples);
                    olm             = ilm * fOutGain;
                }

                if (c->pInMeter != NULL)
                    c->pInMeter->set_value(ilm);
                if (c->pReturnMeter != NULL)
                    c->pReturnMeter->set_value(rlm);
                if (c->pOutMeter != NULL)
                    c->pOutMeter->set_value(olm);
            }
        }

        void Return::dump(dspu::IStateDumper *v) const
        {
            plug::Module::dump(v);

            v->write("nChannels", nChannels);
            v->begin_array("vChannels", vChannels, nChannels);
            {
                for (size_t i=0; i<nChannels; ++i)
                {
                    const channel_t *c = &vChannels[i];

                    v->begin_object(c, sizeof(channel_t));
                    {
                        v->write_object("sBypass", &c->sBypass);

                        v->write("pIn", c->pIn);
                        v->write("pOut", c->pOut);
                        v->write("pReturn", c->pReturn);

                        v->write("pInMeter", c->pInMeter);
                        v->write("pOutMeter", c->pOutMeter);
                        v->write("pReturnMeter", c->pReturnMeter);
                    }
                    v->end_object();
                }
            }
            v->end_array();

            v->write("fInGain", fInGain);
            v->write("fOutGain", fOutGain);
            v->write("fReturnGain", fReturnGain);
            v->write("enMode", int(enMode));

            v->write("pBypass", pBypass);
            v->write("pInGain", pInGain);
            v->write("pOutGain", pOutGain);
            v->write("pMode", pMode);
            v->write("pReturnGain", pReturnGain);
        }

    } /* namespace plugins */
} /* namespace lsp */


