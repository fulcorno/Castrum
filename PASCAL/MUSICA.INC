type
    nota = record
              freq : integer ;
              dura : word ;
           end ;

    brano = array [1..15000] of nota ; { largest possible size }

const n_fine:nota=(freq:-1; dura:0) ;
      n_capo:nota=(freq: 0; dura:0) ;
      n_lega:nota=(freq:-3; dura:0) ;
      n_stac:nota=(freq:-4; dura:0) ;
      n_lest:nota=(freq:-2; dura:0) ;

