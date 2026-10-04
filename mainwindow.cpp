#include "mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFile>
#include <QCoreApplication>
#include <sstream>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      vaultFileName(QCoreApplication::applicationDirPath() + "/vault.enc"),
      initialized(false) {
    setupUI();
    if (!authenticate()) {
        return;
    }
    initialized = true;
    loadVaultToTable();
}
MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    resize(600, 450);
    setWindowTitle("Simple Pass v0.1.0");

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(3);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Сервис" << "Имя пользователя" << "Пароль");
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(tableWidget);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    serviceInput = new QLineEdit(this);
    serviceInput->setPlaceholderText("Сервис");
    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Логин");
    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Пароль");
    passwordInput->setEchoMode(QLineEdit::Password);

    inputLayout->addWidget(serviceInput);
    inputLayout->addWidget(usernameInput);
    inputLayout->addWidget(passwordInput);
    mainLayout->addLayout(inputLayout);

    QHBoxLayout *btnLayout1 = new QHBoxLayout();
    addButton = new QPushButton("Добавить", this);
    editButton = new QPushButton("Изменить пароль", this);
    deleteButton = new QPushButton("Удалить выбранное", this);

    btnLayout1->addWidget(addButton);
    btnLayout1->addWidget(editButton);
    btnLayout1->addWidget(deleteButton);
    mainLayout->addLayout(btnLayout1);

    QHBoxLayout *btnLayout2 = new QHBoxLayout();
    changeMasterButton = new QPushButton("Сменить мастер-пароль", this);
    btnLayout2->addWidget(changeMasterButton);
    mainLayout->addLayout(btnLayout2);

    setCentralWidget(centralWidget);

    connect(addButton, SIGNAL(clicked()), this, SLOT(onAddCredential()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(onEditCredential()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteCredential()));
    connect(changeMasterButton, SIGNAL(clicked()), this, SLOT(onChangeMasterPassword()));
}

bool MainWindow::authenticate() {
    bool ok;
    QString pass = QInputDialog::getText(this, "Вход", "Введите мастер-пароль:", QLineEdit::Password, "", &ok);
    if (!ok || pass.isEmpty()) {
        return false;
    }
    masterPassword = pass;

    QFile file(vaultFileName);
    if (file.exists()) {
        // Если файл существует — обычный вход с проверкой пароля
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл хранилища!");
            return false;
        }
        QByteArray encryptedData = file.readAll();
        file.close();

        QByteArray decryptedData = decrypt(encryptedData, masterPassword);
        if (decryptedData.isEmpty()) {
            QMessageBox::critical(this, "Ошибка", "Неверный мастер-пароль или поврежденный файл!");
            return false;
        }

        std::string content(decryptedData.constData(), decryptedData.size());
        std::stringstream ss(content);
        std::string line;
        while (std::getline(ss, line)) {
            std::stringstream lineStream(line);
            std::string service, username, password;
            if (std::getline(lineStream, service, ',') &&
                std::getline(lineStream, username, ',') &&
                std::getline(lineStream, password)) {
                Credential cred;
                cred.service = service;
                cred.username = username;
                cred.password = password;
                credentials.push_back(cred);
            }
        }
    } else {
        onSaveVault();
    }
    return true;
}
void MainWindow::loadVaultToTable() {
    tableWidget->setRowCount(0);
    for (size_t i = 0; i < credentials.size(); ++i) {
        tableWidget->insertRow(i);
        tableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(credentials[i].service)));
        tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(credentials[i].username)));
        tableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(credentials[i].password)));
    }
}

void MainWindow::onAddCredential() {
    QString s = serviceInput->text();
    QString u = usernameInput->text();
    QString p = passwordInput->text();

    if (s.isEmpty() || u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Заполните все поля!");
        return;
    }

    Credential cred;
    cred.service = s.toStdString();
    cred.username = u.toStdString();
    cred.password = p.toStdString();
    credentials.push_back(cred);

    loadVaultToTable();
    onSaveVault();

    serviceInput->clear();
    usernameInput->clear();
    passwordInput->clear();
}

void MainWindow::onEditCredential() {
    int row = tableWidget->currentRow();
    if (row < 0 || row >= static_cast<int>(credentials.size())) {
        QMessageBox::warning(this, "Внимание", "Выберите строку в таблице для изменения пароля!");
        return;
    }

    bool ok;
    QString newPassword = QInputDialog::getText(this, "Изменение пароля",
        QString("Введите новый пароль для сервиса <b>%1</b>:").arg(QString::fromStdString(credentials[row].service)),
        QLineEdit::Password, "", &ok);

    if (!ok || newPassword.isEmpty()) {
        return;
    }

    credentials[row].password = newPassword.toStdString();
    loadVaultToTable();
    onSaveVault();
    QMessageBox::information(this, "Успех", "Пароль успешно изменен!");
}

void MainWindow::onChangeMasterPassword() {
    bool ok;
    QString oldPass = QInputDialog::getText(this, "Смена мастер-пароля", "Введите текущий мастер-пароль:", QLineEdit::Password, "", &ok);
    if (!ok) return;

    if (oldPass != masterPassword) {
        QMessageBox::critical(this, "Ошибка", "Текущий мастер-пароль введен неверно!");
        return;
    }

    QString newPass1 = QInputDialog::getText(this, "Смена мастер-пароля", "Введите новый мастер-пароль:", QLineEdit::Password, "", &ok);
    if (!ok || newPass1.isEmpty()) return;

    QString newPass2 = QInputDialog::getText(this, "Смена мастер-пароля", "Повторите новый мастер-пароль:", QLineEdit::Password, "", &ok);
    if (!ok || newPass2.isEmpty()) return;

    if (newPass1 != newPass2) {
        QMessageBox::critical(this, "Ошибка", "Новые пароли не совпадают!");
        return;
    }

    masterPassword = newPass1;
    onSaveVault();
    QMessageBox::information(this, "Успех", "Мастер-пароль успешно изменен!");
}

void MainWindow::onDeleteCredential() {
    int row = tableWidget->currentRow();
    if (row < 0 || row >= static_cast<int>(credentials.size())) {
        QMessageBox::warning(this, "Внимание", "Выберите строку для удаления!");
        return;
    }

    credentials.erase(credentials.begin() + row);
    loadVaultToTable();
    onSaveVault();
}

void MainWindow::onSaveVault() {
    std::string content = "";
    for (size_t i = 0; i < credentials.size(); ++i) {
        content += credentials[i].service + "," + credentials[i].username + "," + credentials[i].password + "\n";
    }

    QByteArray plainData(content.c_str(), content.size());
    QByteArray encryptedData = encrypt(plainData, masterPassword);

    if (encryptedData.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Ошибка шифрования данных! Файл хранилища не был изменен.");
        return;
    }

    QFile file(vaultFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл хранилища для записи!");
        return;
    }
    file.write(encryptedData);
    file.close();
}

QByteArray MainWindow::encrypt(const QByteArray &plainText, const QString &masterPass) {
    const EVP_CIPHER *cipher = EVP_aes_256_cbc();
    unsigned char salt[8];
    if (!RAND_bytes(salt, sizeof(salt))) {
        return QByteArray();
    }

    unsigned char key[32];
    unsigned char iv[16];

    QByteArray passBytes = masterPass.toUtf8();
    EVP_BytesToKey(cipher, EVP_sha256(), salt,
                   (unsigned char*)passBytes.constData(),
                   passBytes.length(), 1, key, iv);

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);

    if (!EVP_EncryptInit_ex(&ctx, cipher, NULL, key, iv)) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        return QByteArray();
    }

    QByteArray cipherText;
    cipherText.append("Salted__", 8);
    cipherText.append((const char*)salt, 8);

    int len = 0;
    int ciphertext_len = 0;
    QByteArray outBuf;
    outBuf.resize(plainText.size() + EVP_MAX_BLOCK_LENGTH);

    if (!EVP_EncryptUpdate(&ctx, (unsigned char*)outBuf.data(), &len,
                           (const unsigned char*)plainText.constData(), plainText.size())) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        return QByteArray();
    }
    ciphertext_len = len;

    if (!EVP_EncryptFinal_ex(&ctx, (unsigned char*)outBuf.data() + len, &len)) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        return QByteArray();
    }
    ciphertext_len += len;
    EVP_CIPHER_CTX_cleanup(&ctx);

    outBuf.resize(ciphertext_len);
    cipherText.append(outBuf);
    return cipherText;
}

QByteArray MainWindow::decrypt(const QByteArray &cipherText, const QString &masterPass) {
    if (cipherText.size() < 16 || !cipherText.startsWith("Salted__")) {
        return QByteArray();
    }

    const char *salt = cipherText.constData() + 8;
    const char *actualCipher = cipherText.constData() + 16;
    int actualCipherLen = cipherText.size() - 16;

    const EVP_CIPHER *cipher = EVP_aes_256_cbc();
    unsigned char key[32];
    unsigned char iv[16];

    QByteArray passBytes = masterPass.toUtf8();
    EVP_BytesToKey(cipher, EVP_sha256(), (const unsigned char*)salt,
                   (unsigned char*)passBytes.constData(),
                   passBytes.length(), 1, key, iv);

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);

    if (!EVP_DecryptInit_ex(&ctx, cipher, NULL, key, iv)) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        return QByteArray();
    }

    QByteArray outBuf;
    outBuf.resize(actualCipherLen + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int plaintext_len = 0;

    if (!EVP_DecryptUpdate(&ctx, (unsigned char*)outBuf.data(), &len,
                          (const unsigned char*)actualCipher, actualCipherLen)) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        return QByteArray();
    }
    plaintext_len = len;

    if (!EVP_DecryptFinal_ex(&ctx, (unsigned char*)outBuf.data() + len, &len)) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        return QByteArray();
    }
    plaintext_len += len;
    EVP_CIPHER_CTX_cleanup(&ctx);

    outBuf.resize(plaintext_len);
    return outBuf;
}
