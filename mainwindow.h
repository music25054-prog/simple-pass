#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <vector>
#include <string>

struct Credential {
    std::string service;
    std::string username;
    std::string password;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool isInitialized() const { return initialized; }

private slots:
    void onAddCredential();
    void onDeleteCredential();
    void onEditCredential();
    void onChangeMasterPassword();
    void onSaveVault();

private:
    void setupUI();
    bool authenticate();
    void loadVaultToTable();
    
    QByteArray encrypt(const QByteArray &plainText, const QString &masterPassword);
    QByteArray decrypt(const QByteArray &cipherText, const QString &masterPassword);

    QTableWidget *tableWidget;
    QLineEdit *serviceInput;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *editButton;
    QPushButton *changeMasterButton;

    std::vector<Credential> credentials;
    QString masterPassword;
    QString vaultFileName;
    bool initialized;
};

#ifndef nullptr
#define nullptr 0
#endif

#endif // MAINWINDOW_H
