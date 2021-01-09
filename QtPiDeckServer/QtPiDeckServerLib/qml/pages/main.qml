import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import QtPiDeck 1.0

Window {
    minimumWidth: 1024
    minimumHeight: 720
    visible: true
    title: qsTr("Hello World")

    SettingsViewModel {
        id: settingsViewModel
    }

    Column {
        anchors.centerIn: parent
        spacing: 10

        GroupBox {
            title: "QtPiDeck server settings"
            label: GroupTitle {
                text: parent.title
                x: parent.leftPadding
            }

            ColumnLayout {
                Label {
                    text: "Ip"
                }

                TextField {
                    selectByMouse: true
                    text: settingsViewModel.deckServerAddress
                    onEditingFinished: settingsViewModel.deckServerAddress = text
                }

                Label {
                    text: "Port"
                    id: serverPortLabel
                }

                PortField {
                    text: settingsViewModel.deckServerPort
                    onEditingFinished: settingsViewModel.deckServerPort = text
                }
            }
        }

        GroupBox {
            title: "OBS websocket settings"
            label: GroupTitle {
                text: parent.title
                x: parent.leftPadding
            }

            ColumnLayout {
                Text {
                    text: "Ip"
                }

                TextField {
                    selectByMouse: true
                    text: settingsViewModel.obsWebsocketAddress
                    onEditingFinished: settingsViewModel.obsWebsocketAddress = text
                }

                Text {
                    text: "Port"
                }

                PortField {
                    text: settingsViewModel.obsWebsocketPort
                    onEditingFinished: settingsViewModel.obsWebsocketPort = text
                }
            }
        }

        Button {
            text: qsTr("Save settings")
            onClicked: settingsViewModel.saveSettings();

            anchors.right: parent.right
        }

    }
}


/*##^##
Designer {
    D{i:0;formeditorZoom:1.1;height:1200;width:1920}
}
##^##*/
