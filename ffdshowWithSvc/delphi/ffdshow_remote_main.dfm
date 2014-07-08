object mainForm: TmainForm
  Left = 410
  Top = 420
  Width = 558
  Height = 330
  Caption = 'ffdshow remote control sample'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object btFind: TButton
    Left = 6
    Top = 4
    Width = 97
    Height = 25
    Caption = 'Find ffdshow'
    TabOrder = 0
    OnClick = btFindClick
  end
  object lvFFDSHOW: TListView
    Left = 6
    Top = 33
    Width = 248
    Height = 267
    Anchors = [akLeft, akTop, akBottom]
    Columns = <
      item
        AutoSize = True
        Caption = 'Window handle'
      end
      item
        AutoSize = True
        Caption = 'Application'
      end>
    ColumnClick = False
    HideSelection = False
    ReadOnly = True
    RowSelect = True
    TabOrder = 1
    ViewStyle = vsReport
    OnSelectItem = lvFFDSHOWSelectItem
  end
  object pnl: TPanel
    Left = 256
    Top = 33
    Width = 290
    Height = 268
    BevelOuter = bvNone
    TabOrder = 2
    Visible = False
    object lblCurtime: TLabel
      Left = 129
      Top = 13
      Width = 72
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'lblCurtime'
    end
    object lblDuration: TLabel
      Left = 207
      Top = 13
      Width = 77
      Height = 13
      AutoSize = False
      Caption = 'lblTime'
    end
    object chbLevels: TCheckBox
      Left = 4
      Top = 57
      Width = 129
      Height = 17
      Caption = 'Levels'
      TabOrder = 0
      OnClick = chbLevelsClick
    end
    object btRun: TButton
      Left = 4
      Top = 8
      Width = 56
      Height = 23
      Caption = 'Run'
      TabOrder = 1
      OnClick = btRunClick
    end
    object btStop: TButton
      Left = 65
      Top = 8
      Width = 56
      Height = 23
      Caption = 'Stop'
      TabOrder = 2
      OnClick = btStopClick
    end
    object edPreset: TEdit
      Left = 4
      Top = 84
      Width = 121
      Height = 21
      TabOrder = 3
      Text = 'tv'
    end
    object btPreset: TButton
      Left = 125
      Top = 84
      Width = 67
      Height = 22
      Caption = 'Set preset'
      TabOrder = 4
      OnClick = btPresetClick
    end
  end
  object tmrTime: TTimer
    Enabled = False
    OnTimer = tmrTimeTimer
    Left = 340
    Top = 69
  end
end
