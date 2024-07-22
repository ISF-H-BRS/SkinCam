/****************************************************************
 *                                                              *
 *  This file is part of the ISF SkinCam project.               *
 *                                                              *
 *  Author:                                                     *
 *  Marcel Hasler <mahasler@gmail.com>                          *
 *                                                              *
 *  (c) 2020 - 2022                                             *
 *  Bonn-Rhein-Sieg University of Applied Sciences              *
 *                                                              *
 *  All Rights Reserved. Redistribution is only permitted       *
 *  with express written permission from the copyright holder.  *
 *                                                              *
 ****************************************************************/

#pragma once

#include "imagewidget.h"
#include "polygonselector.h"

#include <QWidget>

#include <memory>

namespace Ui {
    class SelectionWindow;
}

SKINCAM_BEGIN_NAMESPACE();

class SelectionWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SelectionWindow(QWidget* parent = nullptr);
    ~SelectionWindow() override;

    void setAcceptButtonText(const QString& text);
    void setImage(const QImage& image);

signals:
    void visiblilityChanged(bool visible);
    void selectionAccepted(const std::vector<Polygon>& selection);

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void updateButtons();
    void processSelection();

private:
    void updatePixmap();

private:
    std::unique_ptr<Ui::SelectionWindow> m_ui;

    ImageWidget* m_imageWidget = nullptr;
    PolygonSelector* m_polygonSelector = nullptr;

    QImage m_image;
};

SKINCAM_END_NAMESPACE();
