#ifndef GENS_BASESETTINGS_HPP
#define GENS_BASESETTINGS_HPP

#include <any>
#include <vector>
#include <memory>
#include <functional>

#include <QString>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace gens {

using SetPtr=std::shared_ptr<std::any>;

using ToJsonFunction=std::function<QJsonObject(SetPtr)>;
using FromJsonFunction=std::function<std::any(const QJsonObject&)>;

struct BaseSetting
{
QString name;
SetPtr content;
ToJsonFunction toJson;
FromJsonFunction fromJson;
};

} // namespace gens

#endif // GENS_BASESETTINGS_HPP
