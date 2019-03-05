bgiobj /F pergam.byt    PERGIMG         _pergIMG        _IMG_TEXT
bgiobj /F luna.byt      LUNAIMG         _luna           _IMG_TEXT
bgiobj /F sole.byt      SOLEIMG         _sole           _IMG_TEXT
bgiobj /F status.byt    STATIMG         _statusIMG      _IMG_TEXT
bgiobj /F tasti.byt     KEYSIMG         _mascherina     _IMG_TEXT
bgiobj /F fntdim.byt    FDIMIMG         _FntDim         _IMG_TEXT
bgiobj /F horizon.byt   HORZIMG         _horizon        _IMG_TEXT
bgiobj /F watch.byt     WATCIMG         _watchIMG       _IMG_TEXT
bgiobj /F 20x16.byt     FONTIMG         _bigfont        _IMG_TEXT

del imgs.lib
tlib /C imgs.lib +pergimg +lunaimg +soleimg +statimg +keysimg +fdimimg +horzimg +watcimg +fontimg , con

