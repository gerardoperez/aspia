//
// Aspia Project
// Copyright (C) 2019 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef HOST__INPUT_INJECTOR_H
#define HOST__INPUT_INJECTOR_H

#include "base/macros_magic.h"
#include "base/win/scoped_thread_desktop.h"
#include "proto/desktop.pb.h"

#include <QRect>
#include <QThread>

#include <set>

namespace host {

class InputInjector
{
public:
    InputInjector(bool block_input);
    ~InputInjector();

    void injectPointerEvent(const proto::desktop::PointerEvent& event);
    void injectKeyEvent(const proto::desktop::KeyEvent& event);

private:
    void switchToInputDesktop();
    bool isCtrlAndAltPressed();

    base::ScopedThreadDesktop desktop_;

    const bool block_input_;

    std::set<uint32_t> pressed_keys_;
    QPoint prev_mouse_pos_;

    uint32_t prev_mouse_button_mask_ = 0;

    DISALLOW_COPY_AND_ASSIGN(InputInjector);
};

} // namespace host

#endif // HOST__INPUT_INJECTOR_H
