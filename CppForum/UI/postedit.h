#ifndef POSTEDIT_H
#define POSTEDIT_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class PostEdit : public QWidget
{
    Q_OBJECT
public:
    explicit PostEdit(QWidget *parent = nullptr);

signals:
    void AddPost(const QString &title, const QString &content);

private:
    QLabel *titleLabel;
    QLineEdit *titleEdit;
    QLabel *contentLabel;
    QTextEdit *contentEdit;
    QPushButton *postButton;
};

#endif // POSTEDIT_H
