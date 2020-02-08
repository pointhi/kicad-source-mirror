# Can be viewed in: https://ide.kaitai.io/
#
# This file is a formal specification of the binary format used in Altium.
# Files need to manually extracted using a program which can read the Microsoft Compound File Format.
#
# While I do not create a parser using this file, it is still very helpful to understand the binary
# format.

meta:
  id: altium_binary
  endian: le
  encoding: UTF-8

seq:
  - id: record
    type: record
    repeat: eos

# https://github.com/thesourcerer8/altium2kicad/blob/master/convertpcb.pl#L1291
types:
  record:
    seq:
      - id: recordtype
        type: u1
        enum: record_id
      - id: record
        type:
          switch-on: recordtype
          cases:
            record_id::arc6: arc
            record_id::pad6: pad
            record_id::via6: via
            record_id::track6: track
            record_id::text6: text
            record_id::fill6: fill

  arc:
    seq:
    - id: sub1_len
      type: u4
    - id: data
      type: arc_sub1
      size: sub1_len

  arc_sub1:
    seq:
      - id: layer
        type: u1
      - size: 2
      - id: net
        type: u2
      - size: 2
      - id: component
        type: u2
      - size: 4
      - id: center
        type: xy
      - id: radius
        type: u4
      - id: start_angle
        type: f8
      - id: end_angle
        type: f8
      - id: width
        type: u4

  pad:
    seq:
    - id: sub1_len
      type: u4
    - id: designator
      type: pad_sub1
      size: sub1_len
    - id: sub2_len
      type: u4
    - size: sub2_len
    - id: sub3_len
      type: u4
    - size: sub3_len
    - id: sub4_len
      type: u4
    - size: sub4_len
    - id: sub5_len
      type: u4
    - id: size_and_shape
      type: pad_sub5
      size: sub5_len
    - id: sub6_len
      type: u4
    - id: size_and_shape_by_layer
      type: pad_sub6
      size: sub6_len
      if: sub6_len > 0

  pad_sub1:
    seq:
      - id: name_len  # = len-1?
        type: u1
      - id: name
        type: str
        size: name_len

  pad_sub5:
    seq:
      - id: layer  # $pos+23
        type: u1
        enum: layer
      - type: b1
      - id: tent_bottom
        type: b1
      - id: tent_top
        type: b1
      - type: b5
      - size: 1
      - id: net  # $pos+26
        type: u2
      - size: 2
      - id: component  # $pos+30
        type: u2
      - size: 4
      - id: position  # $pos+36, $pos+40
        type: xy
      - id: topsize  # $pos+44, $pos+48
        type: xy
      - id: midsize  # $pos+52, $pos+56
        type: xy
      - id: botsize  # $pos+60, $pos+64
        type: xy
      - id: holesize  # $pos+68
        type: u4
      - id: topshape  # $pos+72
        type: u1
        enum: pad_shape
      - id: midshape  # $pos+73
        type: u1
        enum: pad_shape
      - id: botshape  # $pos+74
        type: u1
        enum: pad_shape
      - id: direction  # $pos+75
        type: f8
      - id: plated  # $pos+83
        type: u1
        enum: boolean
      - size: 1
      - id: pad_mode  # $pos+85
        type: u1
        enum: pad_mode
      - size: 5
      - id: ccw  # $pos+91
        type: u4
      - id: cen  # $pos+95
        type: u1
      - size: 1
      - id: cag  # $pos+97
        type: u4
      - id: cpr  # $pos+101
        type: u4
      - id: cpc  # $pos+105
        type: u4
      - id: pastemaskexpanionmanual
        type: s4
      - id: cse  # $pos+113
        type: u4
      - id: cpl  # $pos+117
        type: u1
      - size: 6
      - id: pastemaskexpansionmode  # $pos+124
        type: u1
        enum: pad_mode
      - id: soldermaskexpansionmode  # $pos+125
        type: u1
        enum: pad_mode
      - size: 3
      - id: holerotation  # $pos+129
        type: f8
      - size: 4
      - id: testpoint_assembly_top
        type: u1
        enum: boolean
      - id: testpoint_assembly_bottom
        type: u1
        enum: boolean

  pad_sub6:
    seq:
    - id: x
      type: s4
      repeat: expr
      repeat-expr: 29
    - id: y
      type: s4
      repeat: expr
      repeat-expr: 29
    - id: shape
      type: u1
      enum: pad_shape
      repeat: expr
      repeat-expr: 29
    - size: 1
    - id: hole_type
      type: u1
      enum: pad_hole_type
    - id: slot_length
      type: s4
    - id: slot_rotation
      type: f8
    - id: holeoffset_x
      type: s4
      repeat: expr
      repeat-expr: 32
    - id: holeoffset_y
      type: s4
      repeat: expr
      repeat-expr: 32
    - size: 1
    - id: shape_alt
      type: u1
      enum: pad_shape_alt
      repeat: expr
      repeat-expr: 32
    - id: corner_radius
      type: u1
      repeat: expr
      repeat-expr: 32
    - size: 32

  via:
    seq:
    - id: sub1_len
      type: u4
    - id: data
      type: via_sub1
      size: sub1_len

  via_sub1:
    seq:
      - size: 3
      - id: net
        type: u2
      - size: 8
      - id: pos  # 13
        type: xy
      - id: diameter # 21
        type: s4
      - id: holesize # 29
        type: s4

  track:
    seq:
    - id: sub1_len
      type: u4
    - id: data
      type: track_sub1
      size: sub1_len

  track_sub1:
    seq:
      - id: layer
        type: u1
        enum: layer
      - size: 2
      - id: net
        type: u2
      - size: 2
      - id: component
        type: u2
      - size: 4
      - id: start  # 13
        type: xy
      - id: end  # 21
        type: xy
      - id: width # 29
        type: s4

  text:
    seq:
    - id: sub1_len
      type: u4
    - id: properties
      type: text_sub1
      size: sub1_len
    - id: sub2_len
      type: u4
    - id: text
      type: text_sub2
      size: sub2_len

  text_sub1:
    seq:
      - id: layer
        type: u1
      - size: 2
      - id: net
        type: u2
      - size: 2
      - id: component
        type: u2
      - size: 4
      - id: pos
        type: xy
      - id: height
        type: u4
      - id: font_name_id
        type: u1
      - size: 1
      - id: rotation
        type: f8
      - id: mirrored
        type: u1
        enum: boolean
      - id: strokewidth
        type: u4
      - id: is_comment
        type: u1
        enum: boolean
      - id: is_designator
        type: u1
        enum: boolean
      - size: 90
      - id: position
        type: u1
        enum: text_position

  text_sub2:
    seq:
      - id: len
        type: u1
      - id: name
        type: str
        size: len

  fill:
    seq:
    - id: sub1_len
      type: u4
    - id: data
      type: fill_sub1
      size: sub1_len

  fill_sub1:
    seq:
      - id: layer
        type: u1
      - size: 2
      - id: net
        type: u2
      - size: 8
      - id: pos1
        type: xy
      - id: pos2
        type: xy
      - id: rotation
        type: f8

  xy:
    seq:
      - id: x
        type: s4
      - id: y
        type: s4

enums:
  record_id:
    0x01: arc6
    0x02: pad6
    0x03: via6
    0x04: track6
    0x05: text6
    0x06: fill6

  boolean:
    0: false
    1: true

  pad_shape:
    0: unknown
    1: circle
    2: rect
    3: octagonal

  pad_shape_alt:
    0: unknown
    1: round
    2: rect
    3: octagonal
    9: roundrectangle

  pad_hole_type:
    0: normal
    2: slot

  pad_mode:
    0: simple
    1: top_middle_bottom
    2: full_stack

  enum: pad_mode:
    0: unknown
    1: rule
    2: manual

  text_position:
    1: left_top
    2: left_center
    3: left_bottom
    4: center_top
    5: center_center
    6: center_bottom
    7: right_top
    8: right_center
    9: right_bottom

  layer:
    1: f_cu
    32: b_cu