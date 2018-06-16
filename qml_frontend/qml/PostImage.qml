Image {
    id: img%1;

    source: '%2';

    // FIXME: test on mobile devices
    // FIXME: investigate sourceSize behaviour
    // sourceSize.width: dp(200);
    // sourceSize.height: dp(200);

    width: (sourceSize.width > dp(200)) ? dp(200) : sourceSize.width;
    height: (sourceSize.height > dp(200)) ? dp(200) : sourceSize.height;
}
