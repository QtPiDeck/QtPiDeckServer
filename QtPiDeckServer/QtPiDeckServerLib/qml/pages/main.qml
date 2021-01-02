import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QtPiDeck 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    SettingsViewModel {
        id: settingsViewModel
    }

    ColumnLayout {
        TextField {
            id: addressField
            selectByMouse: true
            text: settingsViewModel.deckServerAddress
            onEditingFinished: settingsViewModel.deckServerAddress = text
        }

        PortField {
            text: settingsViewModel.deckServerPort
            onEditingFinished: settingsViewModel.deckServerPort = text
        }

        Button {
            text: "Press me!"
            onClicked: settingsViewModel.saveSettings();
        }
    }
}

