object TPCMainForm: TTPCMainForm
  Left = 23
  Top = 21
  ActiveControl = CloseButton
  BorderStyle = bsDialog
  Caption = '画像フォーマットコンバータ'
  ClientHeight = 382
  ClientWidth = 547
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ＭＳ Ｐゴシック'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefault
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object Label1: TLabel
    Left = 8
    Top = 9
    Width = 283
    Height = 12
    Caption = '変換したいファイルをこのウィンドウにドロップしてください'
  end
  object TranspOuputFormatGroupBox: TGroupBox
    Left = 248
    Top = 32
    Width = 289
    Height = 201
    Caption = '透過情報を持った画像の出力形式(&F)'
    TabOrder = 1
    object TranspMainFormatLabel: TLabel
      Left = 24
      Top = 116
      Width = 75
      Height = 12
      Caption = 'メイン形式(&C) :'
      FocusControl = TranspMainFormatComboBox
    end
    object TranspMaskFormatLabel: TLabel
      Left = 24
      Top = 139
      Width = 79
      Height = 12
      Caption = 'マスク形式(&M) :'
      FocusControl = TranspMaskFormatComboBox
    end
    object TranspFullTranspColorMethodLabel: TLabel
      Left = 16
      Top = 168
      Width = 146
      Height = 12
      Caption = '完全透明部分の色情報(&U) : '
      FocusControl = TranspFullTranspColorMethodComboBox
    end
    object TranspBMPFormatRadioButton: TRadioButton
      Left = 16
      Top = 24
      Width = 257
      Height = 17
      Caption = '32bit B&MP (メイン+マスク)'
      TabOrder = 0
      OnClick = TranspBMPFormatRadioButtonClick
    end
    object TranspPNGFormatRadioButton: TRadioButton
      Left = 16
      Top = 40
      Width = 257
      Height = 17
      Caption = 'αチャネル付き P&NG (メイン+マスク)'
      Checked = True
      TabOrder = 1
      TabStop = True
      OnClick = TranspPNGFormatRadioButtonClick
    end
    object TranspSeparatedFormatRadioButton: TRadioButton
      Left = 16
      Top = 88
      Width = 257
      Height = 17
      Caption = 'メイン/マスク分離形式(&S)'
      TabOrder = 4
      OnClick = TranspSeparatedFormatRadioButtonClick
    end
    object TranspMainFormatComboBox: TComboBox
      Left = 112
      Top = 112
      Width = 57
      Height = 20
      Style = csDropDownList
      ItemHeight = 12
      TabOrder = 5
      OnChange = TranspMainFormatComboBoxChange
      Items.Strings = (
        'BMP'
        'JPEG'
        'PNG')
    end
    object TranspMaskFormatComboBox: TComboBox
      Left = 112
      Top = 135
      Width = 57
      Height = 20
      Style = csDropDownList
      ItemHeight = 12
      TabOrder = 7
      OnChange = TranspMaskFormatComboBoxChange
      Items.Strings = (
        'BMP'
        'JPEG'
        'PNG')
    end
    object TranspMainJPEGOptionButton: TButton
      Left = 176
      Top = 112
      Width = 97
      Height = 20
      Caption = 'JPEGオプション ...'
      TabOrder = 6
      OnClick = TranspMainJPEGOptionButtonClick
    end
    object TranspMaskJPEGOptionButton: TButton
      Left = 176
      Top = 135
      Width = 97
      Height = 20
      Caption = 'JPEGオプション ...'
      TabOrder = 8
      OnClick = TranspMaskJPEGOptionButtonClick
    end
    object TranspFullTranspColorMethodComboBox: TComboBox
      Left = 176
      Top = 164
      Width = 97
      Height = 20
      Style = csDropDownList
      ItemHeight = 12
      TabOrder = 9
      Items.Strings = (
        '除去'
        'そのまま'
        '合成(1pixel)'
        '合成(2pixel)'
        '合成(3pixel)'
        '合成(5pixel)'
        '合成(8pixel)')
    end
    object TranspTLG5FormatRadioButton: TRadioButton
      Left = 16
      Top = 56
      Width = 257
      Height = 17
      Caption = 'αチャネル付き T&LG5 (メイン+マスク)'
      TabOrder = 2
      OnClick = TranspTLG5FormatRadioButtonClick
    end
    object TranspTLG6FormatRadioButton: TRadioButton
      Left = 16
      Top = 72
      Width = 257
      Height = 17
      Caption = 'αチャネル付き TL&G6 (メイン+マスク)'
      TabOrder = 3
      OnClick = TranspTLG6FormatRadioButtonClick
    end
  end
  object OutputFolderGroup: TGroupBox
    Left = 8
    Top = 240
    Width = 529
    Height = 105
    Caption = '出力フォルダ(&O)'
    TabOrder = 2
    object SameFolderRadioButton: TRadioButton
      Left = 16
      Top = 24
      Width = 249
      Height = 17
      Caption = '入力ファイルと同じフォルダ(&I)'
      Checked = True
      TabOrder = 0
      TabStop = True
      OnClick = SameFolderRadioButtonClick
    end
    object SpecifyFolderRadioButton: TRadioButton
      Left = 16
      Top = 48
      Width = 137
      Height = 17
      Caption = '指定フォルダ(&Y) :'
      TabOrder = 1
      OnClick = SpecifyFolderRadioButtonClick
    end
    object OutputFolderEdit: TEdit
      Left = 160
      Top = 46
      Width = 289
      Height = 20
      Font.Charset = SHIFTJIS_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'ＭＳ ゴシック'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 2
    end
    object OutputFolderRefButton: TButton
      Left = 456
      Top = 46
      Width = 49
      Height = 20
      Caption = '参照 ...'
      TabOrder = 3
      OnClick = OutputFolderRefButtonClick
    end
    object OverwriteCheckBox: TCheckBox
      Left = 16
      Top = 72
      Width = 313
      Height = 17
      Caption = '同名のファイルがあった場合に上書きする(&O)'
      TabOrder = 4
    end
  end
  object CloseButton: TButton
    Left = 460
    Top = 352
    Width = 75
    Height = 21
    Cancel = True
    Caption = '閉じる(&X)'
    TabOrder = 3
    OnClick = CloseButtonClick
  end
  object OpaqueOutputFormatGroupBox: TGroupBox
    Left = 8
    Top = 32
    Width = 233
    Height = 201
    Caption = '透過情報を持たない画像の出力形式(&Q)'
    TabOrder = 0
    object OpaqueBMPFormatRadioButton: TRadioButton
      Left = 16
      Top = 24
      Width = 113
      Height = 17
      Caption = '24bit &BMP'
      TabOrder = 0
      OnClick = OpaqueBMPFormatRadioButtonClick
    end
    object OpaquePNGFormatRadioButton: TRadioButton
      Left = 16
      Top = 40
      Width = 113
      Height = 17
      Caption = '&PNG'
      Checked = True
      TabOrder = 1
      TabStop = True
      OnClick = OpaquePNGFormatRadioButtonClick
    end
    object OpaqueTLG5FormatRadioButton: TRadioButton
      Left = 16
      Top = 56
      Width = 113
      Height = 17
      Caption = '&TLG5'
      TabOrder = 2
      OnClick = OpaqueTLG5FormatRadioButtonClick
    end
    object OpaqueJPEGFormatRadioButton: TRadioButton
      Left = 16
      Top = 88
      Width = 97
      Height = 17
      Caption = '&JPEG'
      TabOrder = 4
      OnClick = OpaqueJPEGFormatRadioButtonClick
    end
    object OpaqueJPEGOptionButton: TButton
      Left = 120
      Top = 88
      Width = 97
      Height = 20
      Caption = 'JPEGオプション ...'
      TabOrder = 5
      OnClick = OpaqueJPEGOptionButtonClick
    end
    object OpaqueTLG6FormatRadioButton: TRadioButton
      Left = 16
      Top = 72
      Width = 97
      Height = 17
      Caption = 'TL&G6'
      TabOrder = 3
      OnClick = OpaqueTLG6FormatRadioButtonClick
    end
  end
  object DoNotShowLogWhenNoErrorCheckBox: TCheckBox
    Left = 8
    Top = 353
    Width = 361
    Height = 17
    Caption = 'エラーが発生しなかった場合はログを表示しない(&K)'
    TabOrder = 4
  end
  object ExpandButton: TCheckBox
    Left = 312
    Top = 6
    Width = 129
    Height = 17
    Caption = 'オプションを表示(&V)'
    TabOrder = 5
    OnClick = ExpandButtonClick
  end
end
