#ifndef NOTESITEMWIDGET_H
#define NOTESITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QTimer>
#include "notesdatatypes.h"

class NotesItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotesItemWidget(const NoteData &note, QWidget *parent = nullptr);
    ~NotesItemWidget();
    
    int noteId() const { return m_noteId; }
    void setNoteData(const NoteData &note);
    NoteData getNoteData() const;
    void setFlashing(bool flashing);
    void startFlashing();
    void stopFlashing();
    void enterEditMode();
    void exitEditMode();

signals:
    void deleteRequested(int noteId);
    void dataChanged(int noteId);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onDeleteClicked();
    void onEditClicked();
    void onSaveClicked();
    void onCancelClicked();
    void onReminderToggled(bool checked);
    void onFlashTimer();

private:
    void setupUi();
    void setupConnections();
    void updateAppearance();
    QString getFirstLine(const QString &content) const;
    void updateColorButtons();
    QString renderMarkdown(const QString &markdown);
    QString applyInlineFormatting(const QString &text);

    int m_noteId;
    QString m_title;
    QString m_content;
    NoteColor m_color;
    bool m_hasReminder;
    QDateTime m_reminderTime;
    bool m_isFlashing;
    bool m_flashState;
    QTimer *m_flashTimer;
    bool m_isEditing;
    bool m_isDragging;
    QPoint m_dragPosition;
    
    QWidget *m_viewWidget;
    QWidget *m_editWidget;
    
    QLabel *m_titleLabel;
    QLabel *m_contentLabel;
    QLabel *m_reminderLabel;
    QPushButton *m_deleteBtn;
    QPushButton *m_editBtn;
    QLabel *m_colorIndicator;
    
    QLineEdit *m_titleEdit;
    QTextEdit *m_contentEdit;
    QList<QPushButton*> m_colorButtons;
    QCheckBox *m_reminderCheck;
    QDateTimeEdit *m_reminderDateTime;
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;
};

#endif
