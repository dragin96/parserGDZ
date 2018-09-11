/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>

#include "httpwindow.h"
#include "ui_authenticationdialog.h"

#if QT_CONFIG(ssl)
const char defaultUrl[] = "https://gdz.ru/class-1/chelovek-i-mir/trafimova/";
#else
const char defaultUrl[] = "http://gdz.ru/class-1/chelovek-i-mir/trafimova/";
#endif
const char defaultFileName[] = "index.html";
const char defaultServerUrl[] = "http://185.20.225.209:1080/getTasks";
ProgressDialog::ProgressDialog(const QUrl &url, QWidget *parent)
    : QProgressDialog(parent)
{
    setWindowTitle(tr("Download Progress"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setLabelText(tr("Downloading %1.").arg(url.toDisplayString()));
    setMinimum(0);
    setValue(0);
    setMinimumDuration(0);
    setMinimumSize(QSize(400, 75));
}

void ProgressDialog::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    setMaximum(totalBytes);
    setValue(bytesRead);
}

HttpWindow::HttpWindow(QWidget *parent)
    : QDialog(parent)
    , statusLabel(new QLabel(tr("Please enter the URL of a gdz book.\n\n"), this))
    , serverUrlLineEdit(new QLineEdit(defaultServerUrl))
    , urlLineEdit(new QLineEdit(defaultUrl))
    , downloadButton(new QPushButton(tr("Download")))
    // , launchCheckBox(new QCheckBox("Launch file"))
    // , defaultFileLineEdit(new QLineEdit(defaultFileName))
    , downloadDirectoryLineEdit(new QLineEdit)
    , reply(nullptr)
    , file(nullptr)
    , httpRequestAborted(false)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("gdz parser"));

    connect(&qnam, &QNetworkAccessManager::authenticationRequired,
            this, &HttpWindow::slotAuthenticationRequired);
#ifndef QT_NO_SSL
    connect(&qnam, &QNetworkAccessManager::sslErrors,
            this, &HttpWindow::sslErrors);
#endif

    QFormLayout *formLayout = new QFormLayout;
    urlLineEdit->setClearButtonEnabled(true);
    connect(urlLineEdit, &QLineEdit::textChanged,
            this, &HttpWindow::enableDownloadButton);
    formLayout->addRow(tr("&URL:"), urlLineEdit);

    serverUrlLineEdit->setClearButtonEnabled(true);
    connect(serverUrlLineEdit, &QLineEdit::textChanged,
            this, &HttpWindow::enableDownloadButton);
    formLayout->addRow(tr("&server URL:"), serverUrlLineEdit);
    QString downloadDirectory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (downloadDirectory.isEmpty() || !QFileInfo(downloadDirectory).isDir())
        downloadDirectory = QDir::currentPath();
    downloadDirectoryLineEdit->setText(QDir::toNativeSeparators(downloadDirectory));
    formLayout->addRow(tr("&Download directory:"), downloadDirectoryLineEdit);
    //formLayout->addRow(tr("Default &file:"), defaultFileLineEdit);
    //launchCheckBox->setChecked(true);
    //formLayout->addRow(launchCheckBox);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);

    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));

    statusLabel->setWordWrap(true);
    mainLayout->addWidget(statusLabel);

    downloadButton->setDefault(true);
    connect(downloadButton, &QAbstractButton::clicked, this, &HttpWindow::downloadFile);
    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(downloadButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);
    this->loop = new QEventLoop;
    this->msgBox = new QMessageBox;
    this->msgBox->setText("Идет построение списка решений");
    this->msgBox->setStandardButtons(QMessageBox::NoButton);
    this->msgBox->setDefaultButton(QMessageBox::NoButton);
    urlLineEdit->setFocus();
}

void HttpWindow::getJson(){

    QString js = "{\"url\":\"" + urlLineEdit->text() +"\"}";

    QByteArray postData = js.toUtf8();

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    QUrl serviceUrl = QUrl( serverUrlLineEdit->text());
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyJsonFinished(QNetworkReply*)));

    QNetworkRequest request(serviceUrl);
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json");
    networkManager->post(request, postData);

    this->msgBox->show();




}
void HttpWindow::replyJsonFinished(QNetworkReply *reply){

    this->msgBox->hide();

    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    QJsonObject rootObj = document.object();

    QString book_name = rootObj["book_name"].toString();
    qDebug() << "book_name" << book_name;
    if(book_name == ""){
        QMessageBox::information(this, tr("Error"),
                                 tr("Bad server response"));
        return;
    }

    QJsonObject tasksObj = rootObj["tasks"].toObject();
    qDebug() << " tasksObj" << tasksObj;
    foreach(const QString& key, tasksObj.keys()) {


        QJsonObject tasks_keys_val = tasksObj.value(key).toObject();
        qDebug() << "Key = " << key;
        //qDebug() << "value = " << tasksObj.value(key);
        //QJsonArray array = tasksObj.value(key);


        QString current_dir = key;
        qDebug() << "KEYS" << tasks_keys_val.keys();
        foreach(const QString& tasks_key, tasks_keys_val.keys()) {


            qDebug() << "tasks_key" << tasks_key;
            int var_i=0;
            foreach (const QJsonValue taskUrl, tasks_keys_val.value(tasks_key).toArray()) {
                QString fileName = tasks_key;
                ++var_i;
                if(tasks_keys_val.value(tasks_key).toArray().size() > 1){
                    fileName=  fileName + "(" + QString::number(var_i) + ")";
                }
                QStringList splits = taskUrl.toString().split(".");
                QString file_extension = splits[splits.length()-1];
                fileName = fileName + "." + file_extension;


                url =  QUrl(taskUrl.toString());
                qDebug() << "url = " << url << "filename = " << fileName << "current_dir" << current_dir << book_name ;
                this->suspendFlag = false;
                download_url(url, fileName, current_dir, book_name);

                /*while(true){
                    QThread::msleep(1500);
                    qDebug() << this->suspendFlag;
                    if (this->suspendFlag) {
                        qDebug() << "ОПУСТИЛИ ФЛАГ";
                        break;
                    }
                }*/
                this->loop->exec();
            }
        }
    }

}

void HttpWindow::download_url(QUrl newUrl, QString fileName, QString current_dir, QString book_name){

    if (!newUrl.isValid()) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Invalid URL: %1").arg(newUrl.errorString()));
        return;
    }


    if (fileName.isEmpty())
        fileName = defaultFileName;
    book_name.replace(QRegExp("[/\\:?\*\"<>]"), "");
    current_dir.replace("/", "\\");
    QString parent_dir = downloadDirectoryLineEdit->text().trimmed();


    if(!QDir(parent_dir).exists()){
        qDebug() << "qdir dont exist";
        QMessageBox::information(this, tr("Error"),
                                 tr("Dir not found: %1").arg(parent_dir));
        return;
    }

    QString book_name_dir = parent_dir + "\\" + book_name;

    qDebug() << "check book_name_dir" << book_name_dir;
    if(!QDir(book_name_dir).exists()){
        qDebug() << "book_name_dir dont exist, create" << book_name_dir;
        QDir().mkdir(book_name_dir);
    }


    qDebug() << "check book_name_sub_dir" << book_name_dir + "\\" + current_dir;
    if(current_dir.split("\\").length()>0 && !QDir(book_name_dir + "\\" + current_dir.split("\\")[0]).exists()){
        qDebug() << "book_name_dir + current_dir dont exist, create" << book_name_dir + "\\" +  current_dir.split("\\")[0];
        QDir().mkdir(book_name_dir + "\\" +  current_dir.split("\\")[0]);
    }

    QString full_path = book_name_dir + "\\" + current_dir;

    qDebug() << "check full_path" << full_path;
    if(!QDir(full_path).exists()){
        qDebug() << "full_path dont exist, create" << full_path;
        QDir().mkdir(full_path);
    }
    QString downloadDirectory = QDir::cleanPath(full_path);
    qDebug() << "parent_dir" << parent_dir;
    qDebug() << "full_path" << full_path;


    bool useDirectory = !downloadDirectory.isEmpty() && QFileInfo(downloadDirectory).isDir();
    if (useDirectory)
        fileName.prepend(downloadDirectory + '/');
    if (QFile::exists(fileName)) {
        if (QMessageBox::question(this, tr("Overwrite Existing File"),
                                  tr("There already exists a file called %1%2."
                                     " Overwrite?")
                                  .arg(fileName,
                                       useDirectory
                                       ? QString()
                                       : QStringLiteral(" in the current directory")),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No)
                == QMessageBox::No) {
            //this->loop->exit();
            return;
        }
        QFile::remove(fileName);
    }

    file = openFileForWrite(fileName);
    if (!file){
        //this->loop->exit();
        return;
    }

    downloadButton->setEnabled(false);

    // schedule the request
    startRequest(newUrl);
}
void HttpWindow::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl;
    httpRequestAborted = false;

    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &HttpWindow::httpFinished);
    //connect(reply, &QNetworkReply::finished, this, &HttpWindow::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &HttpWindow::httpReadyRead);

    ProgressDialog *progressDialog = new ProgressDialog(url, this);
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(progressDialog, &QProgressDialog::canceled, this, &HttpWindow::cancelDownload);
    connect(reply, &QNetworkReply::downloadProgress, progressDialog, &ProgressDialog::networkReplyProgress);
    connect(reply, &QNetworkReply::finished, progressDialog, &ProgressDialog::hide);
    progressDialog->show();

    statusLabel->setText(tr("Downloading %1...").arg(url.toString()));
}

void HttpWindow::downloadFile()
{
    getJson();
}

QFile *HttpWindow::openFileForWrite(const QString &fileName)
{
    QScopedPointer<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file->errorString()));
        return nullptr;
    }
    return file.take();
}

void HttpWindow::cancelDownload()
{
    statusLabel->setText(tr("Download canceled."));
    httpRequestAborted = true;
    reply->abort();
    downloadButton->setEnabled(true);
}

void HttpWindow::httpFinished()
{
    this->loop->quit();
    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        file->close();
        delete file;
        file = nullptr;
    }

    if (httpRequestAborted) {
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    if (reply->error()) {
        QFile::remove(fi.absoluteFilePath());
        statusLabel->setText(tr("Download failed:\n%1.").arg(reply->errorString()));
        downloadButton->setEnabled(true);
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = nullptr;

    if (!redirectionTarget.isNull()) {
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("Redirect"),
                                  tr("Redirect to %1 ?").arg(redirectedUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            QFile::remove(fi.absoluteFilePath());
            downloadButton->setEnabled(true);
            statusLabel->setText(tr("Download failed:\nRedirect rejected."));
            return;
        }
        file = openFileForWrite(fi.absoluteFilePath());
        if (!file) {
            downloadButton->setEnabled(true);
            return;
        }
        startRequest(redirectedUrl);
        return;
    }

    statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3")
                         .arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));

    downloadButton->setEnabled(true);
    this->suspendFlag = true;
    //this->loop->exit();
}

void HttpWindow::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}

void HttpWindow::enableDownloadButton()
{
    downloadButton->setEnabled(!urlLineEdit->text().isEmpty());
}

void HttpWindow::slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
    QDialog authenticationDialog;
    Ui::Dialog ui;
    ui.setupUi(&authenticationDialog);
    authenticationDialog.adjustSize();
    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm(), url.host()));

    // Did the URL have information? Fill the UI
    // This is only relevant if the URL-supplied credentials were wrong
    ui.userEdit->setText(url.userName());
    ui.passwordEdit->setText(url.password());

    if (authenticationDialog.exec() == QDialog::Accepted) {
        authenticator->setUser(ui.userEdit->text());
        authenticator->setPassword(ui.passwordEdit->text());
    }
}

#ifndef QT_NO_SSL
void HttpWindow::sslErrors(QNetworkReply *, const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    if (QMessageBox::warning(this, tr("SSL Errors"),
                             tr("One or more SSL errors has occurred:\n%1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}
#endif
