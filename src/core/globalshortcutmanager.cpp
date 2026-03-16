#include "globalshortcutmanager.h"
#include <QDebug>
#include <QApplication>

#ifdef Q_OS_WIN
#include <QAbstractNativeEventFilter>
#endif

GlobalShortcutManager* GlobalShortcutManager::m_instance = nullptr;

#ifdef Q_OS_WIN
class NativeEventFilter : public QAbstractNativeEventFilter {
public:
    GlobalShortcutManager* manager;
    
    NativeEventFilter(GlobalShortcutManager* m) : manager(m) {}
    
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override {
        Q_UNUSED(eventType);
        Q_UNUSED(result);
        MSG* msg = reinterpret_cast<MSG*>(message);
        
        if (msg->message == WM_HOTKEY) {
            int id = static_cast<int>(msg->wParam);
            emit manager->shortcutTriggeredById(id);
            return true;
        }
        return false;
    }
};
#endif

GlobalShortcutManager::GlobalShortcutManager(QObject* parent)
    : QObject(parent)
    , m_nextId(0)
{
#ifdef Q_OS_WIN
    QAbstractNativeEventFilter* filter = new NativeEventFilter(this);
    qApp->installNativeEventFilter(filter);
    connect(this, &GlobalShortcutManager::shortcutTriggeredById, this, &GlobalShortcutManager::onShortcutTriggeredById);
#endif
}

GlobalShortcutManager::~GlobalShortcutManager() {
#ifdef Q_OS_WIN
    for (auto it = m_shortcuts.begin(); it != m_shortcuts.end(); ++it) {
        UnregisterHotKey(nullptr, it.value().id);
    }
#endif
}

GlobalShortcutManager* GlobalShortcutManager::instance(QObject* parent) {
    if (!m_instance) {
        m_instance = new GlobalShortcutManager(parent);
    }
    return m_instance;
}

bool GlobalShortcutManager::registerShortcut(const QString& id, const QKeySequence& shortcut) {
#ifdef Q_OS_WIN
    if (shortcut.isEmpty()) {
        qWarning() << "快捷键序列为空";
        return false;
    }
    
    ShortcutInfo info;
    info.id = getNextId();
    info.nativeKey = nativeKeyFromQt(static_cast<Qt::Key>(shortcut[0].key()));
    info.nativeModifiers = 0;
    
    for (int i = 1; i < shortcut.count(); ++i) {
        info.nativeModifiers |= nativeModifierFromQt(static_cast<Qt::Key>(shortcut[i].key()));
    }
    
    if (!RegisterHotKey(nullptr, info.id, info.nativeModifiers, info.nativeKey)) {
        qWarning() << "注册全局快捷键失败:" << id << shortcut.toString();
        return false;
    }
    
    m_shortcuts[id] = info;
    m_idMap[info.id] = id;
    qDebug() << "注册全局快捷键成功:" << id << shortcut.toString();
    return true;
#else
    qDebug() << "全局快捷键仅支持Windows平台";
    return false;
#endif
}

void GlobalShortcutManager::unregisterShortcut(const QString& id) {
#ifdef Q_OS_WIN
    if (m_shortcuts.contains(id)) {
        UnregisterHotKey(nullptr, m_shortcuts[id].id);
        m_idMap.remove(m_shortcuts[id].id);
        m_shortcuts.remove(id);
        qDebug() << "注销全局快捷键:" << id;
    }
#endif
}

void GlobalShortcutManager::updateShortcut(const QString& id, const QKeySequence& shortcut) {
    unregisterShortcut(id);
    registerShortcut(id, shortcut);
}

int GlobalShortcutManager::getNextId() {
    return ++m_nextId;
}

void GlobalShortcutManager::onShortcutTriggeredById(int nativeId) {
    if (m_idMap.contains(nativeId)) {
        emit shortcutTriggered(m_idMap[nativeId]);
    }
}

#ifdef Q_OS_WIN
quint32 GlobalShortcutManager::nativeKeyFromQt(Qt::Key key) {
    switch (key) {
        case Qt::Key_Space: return VK_SPACE;
        case Qt::Key_Tab: return VK_TAB;
        case Qt::Key_Return: return VK_RETURN;
        case Qt::Key_Escape: return VK_ESCAPE;
        case Qt::Key_Backspace: return VK_BACK;
        case Qt::Key_Insert: return VK_INSERT;
        case Qt::Key_Delete: return VK_DELETE;
        case Qt::Key_Home: return VK_HOME;
        case Qt::Key_End: return VK_END;
        case Qt::Key_PageUp: return VK_PRIOR;
        case Qt::Key_PageDown: return VK_NEXT;
        case Qt::Key_CapsLock: return VK_CAPITAL;
        case Qt::Key_NumLock: return VK_NUMLOCK;
        case Qt::Key_ScrollLock: return VK_SCROLL;
        case Qt::Key_F1: return VK_F1;
        case Qt::Key_F2: return VK_F2;
        case Qt::Key_F3: return VK_F3;
        case Qt::Key_F4: return VK_F4;
        case Qt::Key_F5: return VK_F5;
        case Qt::Key_F6: return VK_F6;
        case Qt::Key_F7: return VK_F7;
        case Qt::Key_F8: return VK_F8;
        case Qt::Key_F9: return VK_F9;
        case Qt::Key_F10: return VK_F10;
        case Qt::Key_F11: return VK_F11;
        case Qt::Key_F12: return VK_F12;
        case Qt::Key_A: return 0x41;
        case Qt::Key_B: return 0x42;
        case Qt::Key_C: return 0x43;
        case Qt::Key_D: return 0x44;
        case Qt::Key_E: return 0x45;
        case Qt::Key_F: return 0x46;
        case Qt::Key_G: return 0x47;
        case Qt::Key_H: return 0x48;
        case Qt::Key_I: return 0x49;
        case Qt::Key_J: return 0x4A;
        case Qt::Key_K: return 0x4B;
        case Qt::Key_L: return 0x4C;
        case Qt::Key_M: return 0x4D;
        case Qt::Key_N: return 0x4E;
        case Qt::Key_O: return 0x4F;
        case Qt::Key_P: return 0x50;
        case Qt::Key_Q: return 0x51;
        case Qt::Key_R: return 0x52;
        case Qt::Key_S: return 0x53;
        case Qt::Key_T: return 0x54;
        case Qt::Key_U: return 0x55;
        case Qt::Key_V: return 0x56;
        case Qt::Key_W: return 0x57;
        case Qt::Key_X: return 0x58;
        case Qt::Key_Y: return 0x59;
        case Qt::Key_Z: return 0x5A;
        case Qt::Key_0: return 0x30;
        case Qt::Key_1: return 0x31;
        case Qt::Key_2: return 0x32;
        case Qt::Key_3: return 0x33;
        case Qt::Key_4: return 0x34;
        case Qt::Key_5: return 0x35;
        case Qt::Key_6: return 0x36;
        case Qt::Key_7: return 0x37;
        case Qt::Key_8: return 0x38;
        case Qt::Key_9: return 0x39;
        default: return 0;
    }
}

quint32 GlobalShortcutManager::nativeModifierFromQt(Qt::Key key) {
    quint32 native = 0;
    
    switch (key) {
        case Qt::Key_Control: native |= MOD_CONTROL; break;
        case Qt::Key_Shift: native |= MOD_SHIFT; break;
        case Qt::Key_Alt: native |= MOD_ALT; break;
        case Qt::Key_Meta: native |= MOD_WIN; break;
        default: break;
    }
    
    return native;
}
#endif
