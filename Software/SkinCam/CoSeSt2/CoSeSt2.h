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

#ifndef COSEST2_H
#define COSEST2_H

#include <QSettings>
#include <QVariant>

#include <vector>

// ---------------------------------------------------------------------------------------------- //

namespace CoSeSt {

// ---------------------------------------------------------------------------------------------- //

enum class StorageType { Global, Local };

// ---------------------------------------------------------------------------------------------- //

class EntryBase;
using EntryVector = std::vector<EntryBase*>;

// ---------------------------------------------------------------------------------------------- //

class GroupBase;
using GroupVector = std::vector<GroupBase*>;

// ---------------------------------------------------------------------------------------------- //

class EntryBase
{
public:
    auto getName() const -> const QString& { return m_name; }

protected:
    EntryBase(EntryVector* entries, const char* name, QVariant value)
        : m_name(name), m_value(std::move(value)) { entries->push_back(this); }

    friend class Base;
    void setValue(const QVariant& value) { m_value = value; }
    auto getValue() const -> const QVariant& { return m_value; }

private:
    QString m_name;
    QVariant m_value;
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class Entry : public EntryBase
{
public:
    Entry(EntryVector* entries, const char* name, const T& value)
        : EntryBase(entries, name, value) {}

    void set(const T& value) { setValue(value); }
    auto get() const -> T { return getValue().template value<T>(); }

    auto operator=(const T& value) -> Entry& { set(value); return *this; }
    operator T() const { return get(); }
};

// ---------------------------------------------------------------------------------------------- //

class GroupBase
{
public:
    auto getName() const -> const QString& { return m_name; }

protected:
    explicit GroupBase(GroupVector* groups, const char* name)
        : m_name(name) { groups->push_back(this); }

protected:
    friend class Base;
    EntryVector d_entries;

private:
    QString m_name;
};

// ---------------------------------------------------------------------------------------------- //

class Base
{
    Base(const Base&) = delete;
    Base(Base&&) = delete;

    auto operator=(const Base&) = delete;
    auto operator=(Base&&) = delete;

public:
    void loadFromDisk(QString filename = QString());
    void saveToDisk(QString filename = QString());

protected:
    Base(const char* name, StorageType type)
        : m_name(name), m_type(type) {}

    ~Base() = default;

protected:
    EntryVector d_entries;
    GroupVector d_groups;

private:
    using ProcessEntryFunction = std::function<void(QSettings*, EntryBase*)>;
    void processSettingsFile(QString filename, const ProcessEntryFunction& process);

private:
    QString m_name;
    StorageType m_type;
};

// ---------------------------------------------------------------------------------------------- //

} // End of namespace CoSeSt

// ---------------------------------------------------------------------------------------------- //

#define COSEST_EXPAND(x) x // Workaround for MSVC
#define COSEST_FIRST_ARG(x, ...) x

// ---------------------------------------------------------------------------------------------- //

#define COSEST_GET_INSTANCE(classname)                      \
public:                                                     \
    static auto getInstance() -> classname*                 \
    {                                                       \
        static classname instance;                          \
        return &instance;                                   \
    }                                                       \
                                                            \
    static auto getConstInstance() -> const classname&      \
    {                                                       \
        return *getInstance();                              \
    }                                                       \
private:                                                    \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_BEGIN_BASE_2(classname, storagetype)         \
class classname : public CoSeSt::Base                       \
{                                                           \
protected:                                                  \
    explicit classname(const char* name = #classname)       \
        : CoSeSt::Base(name, storagetype) {}                \
                                                            \
public:                                                     \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_BEGIN_BASE_1(classname)                      \
    COSEST_BEGIN_BASE_2(classname,                          \
                        CoSeSt::StorageType::Local)         \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_BEGIN_BASE_X(a, b, FUNC, ...) FUNC           \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_BEGIN_BASE(...)                              \
    COSEST_EXPAND(                                          \
      COSEST_BEGIN_BASE_X(__VA_ARGS__,                      \
                          COSEST_BEGIN_BASE_2,              \
                          COSEST_BEGIN_BASE_1)              \
                            (__VA_ARGS__))                  \
// ---------------------------------------------------------------------------------------------- //

#define COSEST_END_BASE                                     \
};                                                          \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_BEGIN(...)                                   \
    COSEST_BEGIN_BASE(__VA_ARGS__)                          \
    COSEST_GET_INSTANCE(COSEST_FIRST_ARG(__VA_ARGS__))      \
public:                                                     \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_END COSEST_END_BASE

// ---------------------------------------------------------------------------------------------- //

#define COSEST_BEGIN_GROUP(groupname)                       \
class groupname##Group : public CoSeSt::GroupBase           \
{                                                           \
public:                                                     \
    groupname##Group(CoSeSt::GroupVector* groups,           \
                     const char* name)                      \
        : GroupBase(groups, name) {}                        \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_END_GROUP(groupname)                         \
} groupname { &d_groups, #groupname };                      \
                                                            \
auto get##groupname##Group() -> groupname##Group*           \
    { return &groupname; }                                  \
auto get##groupname##Group() const                          \
          -> const groupname##Group& { return groupname; }  \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_ENTRY_3(type, name, value)                   \
CoSeSt::Entry<type> name { &d_entries, #name, value };      \
void set##name(const type& x) { name.set(x); }              \
auto get##name() const -> type { return name.get(); }       \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_ENTRY_2(type, name)                          \
    COSEST_ENTRY_3(type, name, {})                          \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_ENTRY_X(a, b, c, FUNC, ...) FUNC             \

// ---------------------------------------------------------------------------------------------- //

#define COSEST_ENTRY(...)                                   \
    COSEST_EXPAND(                                          \
      COSEST_ENTRY_X(__VA_ARGS__,                           \
                     COSEST_ENTRY_3,                        \
                     COSEST_ENTRY_2)(__VA_ARGS__))          \

// ---------------------------------------------------------------------------------------------- //

#endif // COSEST2_H
