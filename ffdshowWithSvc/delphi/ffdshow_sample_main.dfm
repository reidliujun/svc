object mainForm: TmainForm
  Left = 351
  Top = 459
  Width = 415
  Height = 294
  Caption = 'ffdshow'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object lblAutoq: TLabel
    Left = 253
    Top = 47
    Width = 38
    Height = 13
    Caption = 'lblAutoq'
  end
  object Label1: TLabel
    Left = 8
    Top = 44
    Width = 223
    Height = 13
    Caption = 'Dblclick on preset name to see its autoq setting'
  end
  object Button1: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Create'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 92
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Destroy'
    Enabled = False
    TabOrder = 1
    OnClick = Button2Click
  end
  object lbxProps: TListBox
    Left = 7
    Top = 60
    Width = 226
    Height = 200
    Anchors = [akLeft, akTop, akRight, akBottom]
    ItemHeight = 13
    TabOrder = 2
    OnDblClick = lbxPropsDblClick
  end
end
