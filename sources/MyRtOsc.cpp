//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2018  Jean Pierre Cimalando
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "MyRtOsc.h"
#include <lo/lo.h>
#include <rtosc/ports.h>
#include <string.h>

struct MyRtOsc::Impl
{
    static MyRtOsc theRtOsc;

    lo_server_thread server_ = nullptr;
    std::unique_ptr<char[]> messagebuf_;
    enum { messagebuf_size = 2048 };

    MessageHandler *handler_ = nullptr;
    void *handler_userdata_ = nullptr;

    void handleIncomingMessage(const char *msg, size_t length);

    static void handleError(int num, const char *msg, const char *where);
    static int handleMethod(const char *path, const char *types,
                            lo_arg **argv, int argc, lo_message msg,
                            void *user_data);
};

MyRtOsc MyRtOsc::Impl::theRtOsc;

MyRtOsc::MyRtOsc()
    : P(new Impl)
{
    P->messagebuf_.reset(new char[Impl::messagebuf_size]);
}

MyRtOsc::~MyRtOsc()
{
    close();
}

MyRtOsc &MyRtOsc::instance()
{
    return Impl::theRtOsc;
}

bool MyRtOsc::open(const char *port)
{
    close();

    lo_server_thread server = lo_server_thread_new(port, &Impl::handleError);
    if (!server)
        return false;

    lo_server_thread_add_method(server, nullptr, nullptr, &Impl::handleMethod, P.get());

    P->server_ = server;
    return true;
}

void MyRtOsc::close()
{
    if (P->server_) {
        lo_server_thread_free(P->server_);
        P->server_ = nullptr;
    }
}

int MyRtOsc::getPort() const
{
    lo_server_thread server = P->server_;
    if (!server)
        return 0;

    return lo_server_thread_get_port(server);
}

std::string MyRtOsc::getUrl() const
{
    lo_server_thread server = P->server_;
    if (!server)
        return std::string();

    char *url = lo_server_thread_get_url(server);
    if (!url)
        return std::string();

    std::string str(url);
    free(url);
    return str;
}

void MyRtOsc::setMessageHandler(MessageHandler *handler, void *userdata)
{
    P->handler_ = handler;
    P->handler_userdata_ = userdata;
}

bool MyRtOsc::start()
{
    lo_server_thread server = P->server_;
    if (!server)
        return false;

    if (lo_server_thread_start(server) != 0)
        return false;

    return true;
}

bool MyRtOsc::stop()
{
    lo_server_thread server = P->server_;
    if (!server)
        return false;

    if (lo_server_thread_stop(server) != 0)
        return false;

    return true;
}

void MyRtOsc::Impl::handleIncomingMessage(const char *msg, size_t length)
{
    if (handler_)
        handler_(msg, length, handler_userdata_);
}

void MyRtOsc::Impl::handleError(int num, const char *msg, const char *where)
{
    fprintf(stderr, "OSC error: %s\n", msg);
}

int MyRtOsc::Impl::handleMethod(const char *path, const char *types,
                                lo_arg **argv, int argc, lo_message msg,
                                void *user_data)
{
    Impl *self = (Impl *)user_data;

    char *buf = self->messagebuf_.get();
    memset(buf, 0, messagebuf_size);

    size_t size = messagebuf_size;
    if (!lo_message_serialise(msg, path, buf, &size))
        return 0;

    if(buf[0]=='/' && strrchr(buf, '/')[1]) {  // same as ZynAddSubFx does
        char *msg = rtosc::Ports::collapsePath(buf);
        self->handleIncomingMessage(msg, rtosc_message_length(buf, messagebuf_size));
    }

    return 0;
}
