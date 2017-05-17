// Tensorflow.cpp
// author: Tobias Watzka, Christian Dins
// created: 2017/04/04
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#include "Tensorflow.h"
#include "base/Factory.h"
#include "Trainer.h"
#include "thread/RunAsThread.h"
#include <stdlib.h>

#ifdef USE_SSI_LEAK_DETECTOR
#include "SSI_LeakWatcher.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

namespace ssi {

    ssi_char_t Tensorflow::ssi_log_name[] = "Tensorflow";
    ssi_char_t Tensorflow::ssi_log_name_static[] = "Tensorflow";

    Tensorflow::Tensorflow(const ssi_char_t *file)
            : _file(0),
              _trainer(0),
              _is_loaded(false),
              _probs(0),
              _handler(0),
              _n_classes(0),
              _merged_sample_dimension(0),
              _consumer_sr(0),
              _consumer_dim(0),
              _consumer_byte(0),
              _consumer_num(0),
              _n_metas(0),
              _metas(0),
              _del_trainer(false),
              ssi_log_level(SSI_LOG_LEVEL_DEFAULT) {

        if (file) {
            if (!OptionList::LoadXML(file, _options)) {
                OptionList::SaveXML(file, _options);
            }
            _file = ssi_strcpy(file);
        }

    }

    Tensorflow::~Tensorflow() {

        if (_file) {
            OptionList::SaveXML(_file, _options);
            delete[] _file;
        }
        releaseTrainer();
    }

    void Tensorflow::setTrainer(Trainer *trainer) {
        loadTrainer(trainer);
    };

    bool Tensorflow::loadTrainer(Trainer *trainer) {


        return true;
    }

    void Tensorflow::releaseTrainer() {

        if (_trainer) {

            delete _handler;
            _handler = 0;

            if (_del_trainer) {
                delete _trainer;
            }
            _trainer = 0;

            delete[] _probs;
            _probs = 0;
            _n_classes = 0;
            _merged_sample_dimension = 0;
            _consumer_sr = 0;
            _consumer_dim = 0;
            _consumer_byte = 0;
            _consumer_num = 0;

            delete[] _metas;
            _metas = 0;
            _n_metas = 0;

            _is_loaded = false;
        }
    }

    int Tensorflow::callTensorflow() {
        return 0;
    }

    void Tensorflow::consume_enter(ssi_size_t stream_in_num,
                                   ssi_stream_t stream_in[]) {

        _n_classes = 4;
        _probs = new ssi_real_t[_n_classes];

        _class_names = new ssi_char_t *[_n_classes];
        /*
        for (ssi_size_t i = 0; i < _n_classes; i++) {
            _class_names[i] = ssi_strcpy (samples.getClassName (i));
        } */
        _class_names[0] = ssi_strcpy("Ambient_Animals");
        _class_names[1] = ssi_strcpy("Rain");
        _class_names[2] = ssi_strcpy("Running_Water");
        _class_names[3] = ssi_strcpy("Traffic");
    }

    bool Tensorflow::callTrainer(ssi_time_t time,
                                 ssi_time_t dur,
                                 ssi_size_t n_streams,
                                 ssi_stream_t stream_in[]) {

        return true;//result
    }

    void Tensorflow::consume(IConsumer::info consume_info,
                             ssi_size_t stream_in_num,
                             ssi_stream_t stream_in[]) {

        ssi_real_t *dataptr = ssi_pcast(ssi_real_t, stream_in[0].ptr);

        tensorflow::Session *session;
        tensorflow::Status status = NewSession(tensorflow::SessionOptions(), &session);
        if (!status.ok()) {
            ssi_wrn("status: %s \n", status.ToString().c_str());
            return;
        }

        tensorflow::GraphDef graph_def;

#if __ANDROID__
        status = ReadTextProto(tensorflow::Env::Default(), "/sdcard/android_xmlpipe/frozen_graph.pb", &graph_def);
#else
        status = ReadTextProto(tensorflow::Env::Default(),
                               "/home/mainuser/code/SSI/mobileSSI/plugins/tensorflow/test_files/frozen_graph.pb",
                               &graph_def);
#endif

        if (!status.ok()) {
            ssi_wrn("status: %s \n", status.ToString().c_str());
            return;
        }

        status = session->Create(graph_def);
        if (!status.ok()) {
            ssi_wrn("status: %s \n", status.ToString().c_str());
            return;
        }

        int number_dim = stream_in[0].dim;
        int number_test = stream_in[0].num; // 4072
        int number_classes = 4;

        tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({number_test, number_dim}));
        auto dst = input_tensor.flat<float>().data();
        for (int i = 0; i < stream_in[0].num; i++) {
            std::copy_n(dataptr + i * number_dim, number_dim, dst);
            dst += number_dim;
        }

        std::vector<std::pair<std::string, tensorflow::Tensor>> inputs = {{"input_TT", input_tensor}};
        std::vector<tensorflow::Tensor> outputs;
        status = session->Run(inputs, {"output_TT"}, {}, &outputs);
        if (!status.ok()) {
            ssi_wrn("status: %s \n", status.ToString().c_str());
            return;
        }

        std::vector<int> number_hits(number_classes, 0);
        for (std::vector<tensorflow::Tensor>::iterator it = outputs.begin(); it != outputs.end(); ++it) {
            auto items = it->shaped<float, 2>({number_test, number_classes});
            for (int i = 0; i < number_test; i++) {
                int arg_max = 0;
                float val_max = items(i, 0);
                for (int j = 0; j < number_classes; j++) {
                    if (items(i, j) > val_max) {
                        arg_max = j;
                        val_max = items(i, j);
                    }
                }
                for (int i = 0; i < number_classes; i++) {
                    if (arg_max == i) {
                        number_hits[i]++;
                    }
                }
            }
        }

        std::string classes[] = {"ambient_animals", "rain", "running_water", "traffic"};
        for (int i = 0; i < number_classes; i++) {
            float accuracy = (float) number_hits[i] / number_test;
            ssi_wrn("accuracy for class %s : %f \n", classes[i].c_str(), accuracy);
            _probs[i] = accuracy;
        }
        session->Close();

        _handler->handle(consume_info.time, consume_info.dur, 4, 0, _probs, _class_names, 0, 0);
    }

    void Tensorflow::consume_flush(ssi_size_t stream_in_num,
                                   ssi_stream_t stream_in[]) {

        delete _probs;

    }

    void Tensorflow::listen_enter() {

        if (!_is_loaded && !loadTrainer()) {
            ssi_wrn("could not load trainer '%s'", _options.trainer);
        }
    }

    bool Tensorflow::update(IEvents &events,
                            ssi_size_t n_new_events,
                            ssi_size_t time_ms) {

        return true;
    }

    void Tensorflow::listen_flush() {}

    bool Tensorflow::setEventListener(IEventListener *listener) {

        if (!listener) {
            return false;
        }

        ssi_size_t eid, sid;

        if (_options.address[0] != '\0') {

            _event_address.setAddress(_options.address);
            sid = Factory::AddString(_event_address.getSender(0));
            eid = Factory::AddString(_event_address.getEvent(0));

        } else {

            ssi_wrn("use of deprecated option 'sname' and 'ename', use 'address' instead")

            sid = Factory::AddString(_options.sname);
            if (sid == SSI_FACTORY_STRINGS_INVALID_ID) {
                return false;
            }
            eid = Factory::AddString(_options.ename);
            if (eid == SSI_FACTORY_STRINGS_INVALID_ID) {
                return false;
            }

            _event_address.setSender(_options.sname);
            _event_address.setEvents(_options.ename);
        }

        _handler = new EventHandler(listener, sid, eid, _options.winner, _options.select);

        return true;
    }

// Class EventHandler
    Tensorflow::EventHandler::EventHandler(IEventListener *listener,
                                           ssi_size_t sid,
                                           ssi_size_t eid,
                                           bool winner_only,
                                           ssi_char_t *select)
            : _listener(listener),
              _winner_only(winner_only),
              _class_ids(0),
              _n_select(0),
              _select(0) {

        if (select && select[0] != '\0') {
            _select = ssi_parse_indices(select, _n_select);
        }

        ssi_event_init(_event, SSI_ETYPE_MAP, sid, eid);
    };

    Tensorflow::EventHandler::~EventHandler() {

        _n_select = 0;
        delete[] _select;
        _select = 0;

        ssi_event_destroy(_event);
        delete[] _class_ids;
        _class_ids = 0;
    }

    void Tensorflow::EventHandler::handle(ssi_time_t time,
                                          ssi_time_t duration,
                                          ssi_size_t n_classes,
                                          ssi_size_t class_index,
                                          const ssi_real_t *probs,
                                          ssi_char_t *const *class_names,
                                          ssi_size_t n_metas,
                                          ssi_real_t *metas) {

        if (_event.tot == 0) {
            _class_ids = new ssi_size_t[n_classes];
            for (ssi_size_t i = 0; i < n_classes; i++) {
                _class_ids[i] = Factory::AddString(class_names[i]);
            }
            if (_winner_only) {
                ssi_event_adjust(_event, sizeof(ssi_event_map_t));
            } else if (_select) {
                ssi_event_adjust(_event, _n_select * sizeof(ssi_event_map_t));

                // check if indices are valid
                for (ssi_size_t i = 0; i < _n_select; i++) {
                    if (_select[i] < 0) {
                        ssi_wrn_static("index '%d' is negative and will be replaced by 0", _select[i]);
                        _select[i] = 0;
                    } else if (_select[i] >= n_classes) {
                        ssi_wrn_static("index '%d' out of range and will be replaced by 0", _select[i]);
                        _select[i] = 0;
                    }
                }

            } else {
                ssi_event_adjust(_event, n_classes * sizeof(ssi_event_map_t));
            }
        }

        ssi_event_map_t *ptr = ssi_pcast(ssi_event_map_t, _event.ptr);
        if (_winner_only) {
            ssi_real_t max_val = probs[0];
            ssi_size_t max_ind = 0;
            for (ssi_size_t i = 1; i < n_classes; i++) {
                if (probs[i] > max_val) {
                    max_val = probs[i];
                    max_ind = i;
                }
            }
            ptr[0].id = _class_ids[max_ind];
            ptr[0].value = probs[max_ind];
        } else if (_select) {
            for (ssi_size_t i = 0; i < _n_select; i++) {
                ptr[i].value = probs[_select[i]];
                ptr[i].id = _class_ids[_select[i]];
            }
        } else {
            for (ssi_size_t i = 0; i < n_classes; i++) {
                ptr[i].value = probs[i];
                ptr[i].id = _class_ids[i];
            }
        }
        _event.time = ssi_cast(ssi_size_t, time * 1000 + 0.5);
        _event.dur = ssi_cast(ssi_size_t, duration * 1000 + 0.5);
        _listener->update(_event);
    };

}
