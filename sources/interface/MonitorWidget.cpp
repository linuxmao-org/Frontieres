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

#include "MonitorWidget.h"
#include <QPainter>
#include <QDebug>
#include <math.h>

MonitorWidget::MonitorWidget(QWidget *parent)
    : QFrame(parent)
{
}

void MonitorWidget::feedSample(float sample)
{
    unsigned w = width();

    if ((int)w <= 0) {
        historyIndex_ = 0;
        historySize_ = 0;
        history_.reset();
    }
    else if (historySize_ != w) {
        historyIndex_ = 0;
        historySize_ = w;
        history_.reset(new float[w]());
    }

    history_[historyIndex_++] = sample;
    if (historyIndex_ == historySize_) historyIndex_ = 0;

    repaint();
}

void MonitorWidget::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter p(this);
    QRect r = rect();

    if (r.width() <= 0 || r.height() <= 0)
        return;

    p.fillRect(r, Qt::black);

    unsigned n = r.width();
    if (historySize_ != n)
        return;

    for (unsigned i = 0; i < n; ++i) {
        float s = history_[(historyIndex_ + i) % n];
        QPoint xy = QPoint(r.x() + i, (int)lroundf((1 - s) * r.height()));
        p.setPen(QColor(0x20, 0x4A, 0x87));
        p.drawLine(QPoint(xy.x(), r.height()), xy);
        p.setPen(QColor(0x72, 0x9F, 0xCF));
        p.drawPoint(xy);
    }
}
