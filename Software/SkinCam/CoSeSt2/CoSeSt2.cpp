/**************************************************************************************************
 *                                                                                                *
 *  This file is part of the CoSeSt2 project.                                                     *
 *                                                                                                *
 *  Author:                                                                                       *
 *  Marcel Hasler <mahasler@gmail.com>                                                            *
 *                                                                                                *
 *  Original concept by Sebastian Sporrer                                                         *
 *                                                                                                *
 *  Copyright (c) 2019-2022                                                                       *
 *  Bonn-Rhein-Sieg University of Applied Sciences                                                *
 *                                                                                                *
 *  Redistribution and use in source and binary forms, with or without modification,              *
 *  are permitted provided that the following conditions are met:                                 *
 *                                                                                                *
 *  1. Redistributions of source code must retain the above copyright notice,                     *
 *     this list of conditions and the following disclaimer.                                      *
 *                                                                                                *
 *  2. Redistributions in binary form must reproduce the above copyright notice,                  *
 *     this list of conditions and the following disclaimer in the documentation                  *
 *     and/or other materials provided with the distribution.                                     *
 *                                                                                                *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"                   *
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED             *
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.            *
 *  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,              *
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT            *
 *  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR            *
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,             *
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)            *
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE                    *
 *  POSSIBILITY OF SUCH DAMAGE.                                                                   *
 *                                                                                                *
 **************************************************************************************************/

#include "CoSeSt2.h"

#include <QCoreApplication>

#include <memory>

// ---------------------------------------------------------------------------------------------- //

void CoSeSt::Base::loadFromDisk(QString filename)
{
    const auto load = [](QSettings* settings, EntryBase* entry) {
        entry->setValue(settings->value(entry->getName(), entry->getValue()));
    };

    processSettingsFile(std::move(filename), load);
}

// ---------------------------------------------------------------------------------------------- //

void CoSeSt::Base::saveToDisk(QString filename)
{
    const auto save = [](QSettings* settings, EntryBase* entry) {
        settings->setValue(entry->getName(), entry->getValue());
    };

    processSettingsFile(std::move(filename), save);
}

// ---------------------------------------------------------------------------------------------- //

void CoSeSt::Base::processSettingsFile(QString filename, const ProcessEntryFunction& process)
{
    std::unique_ptr<QSettings> settings;

    if (m_type == StorageType::Global)
    {
        Q_ASSERT(!QCoreApplication::organizationName().isEmpty());
        Q_ASSERT(!QCoreApplication::applicationName().isEmpty());

        settings = std::make_unique<QSettings>();
    }
    else
    {
        if (filename.isEmpty())
            filename = m_name + ".ini";

        settings = std::make_unique<QSettings>(filename, QSettings::IniFormat);
    }

    for (auto entry : d_entries)
        process(settings.get(), entry);

    for (auto group : d_groups)
    {
        settings->beginGroup(group->getName());

        for (auto entry : group->d_entries)
            process(settings.get(), entry);

        settings->endGroup();
    }
}

// ---------------------------------------------------------------------------------------------- //
