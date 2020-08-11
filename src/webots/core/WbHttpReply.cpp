// Copyright 1996-2020 Cyberbotics Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "WbHttpReply.hpp"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

// Remarks:
// - "Access-Control-Allow-Origin" is to solve this error appearing at least on Chrome:
//    `Error: No 'Access-Control-Allow-Origin' header is present on the requested resource`

QByteArray WbHttpReply::forge404Reply() {
  static QByteArray reply;
  if (reply.isEmpty()) {
    reply.append("HTTP/1.1 404 Not Found\r\n");
    reply.append("Access-Control-Allow-Origin: *\r\n");
  }
  return reply;
}

QByteArray WbHttpReply::forgeHTMLReply(const QString &htmlContent) {
  QByteArray reply;
  reply.append("HTTP/1.1 200 OK\r\n");
  reply.append("Access-Control-Allow-Origin: *\r\n");
  reply.append("Content-Type: text/html\r\n");
  reply.append(QString("Content-Length: %1\r\n").arg(htmlContent.length()));
  reply.append("\r\n");
  reply.append(htmlContent);
  return reply;
}

QByteArray WbHttpReply::forgeFileReply(const QString &fileName) {
  QByteArray reply;

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return forge404Reply();

  const QByteArray data = file.readAll();
  QFileInfo fi(file);
  const QString extension = fi.suffix().toLower();
  QString type;
  if (extension == "png" || extension == "jpg" || extension == "jpeg")
    type = QString("image/%1").arg(extension);
  else if (extension == "html" || extension == "css")
    type = QString("text/%1").arg(extension);
  else if (extension == "js")
    type = "application/javascript";
  reply.append("HTTP/1.1 200 OK\r\n");
  reply.append("Access-Control-Allow-Origin: *\r\n");
  reply.append("Cache-Control: public, max-age=3600\r\n");  // Help the browsers to cache the file for 1 hour.
  reply.append(QString("Content-Type: %1\r\n").arg(type));
  reply.append(QString("Content-Length: %1\r\n").arg(data.length()));
  reply.append("\r\n");
  reply.append(data);

  return reply;
}
