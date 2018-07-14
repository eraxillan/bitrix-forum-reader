import QtQuick 2.10;

Image {
    id: img_6d63cf96fdc747d591311cfa352b26e4;

    source: '_5b4898dbfff046a7ad6474e9b9155808';

    // FIXME: test on mobile devices
    // FIXME: investigate sourceSize behaviour
    // sourceSize.width: dp(200);
    // sourceSize.height: dp(200);

    width: (sourceSize.width > dp(200)) ? dp(200) : sourceSize.width;
    height: (sourceSize.height > dp(200)) ? dp(200) : sourceSize.height;
}
