object MainForm: TMainForm
  Left = 56
  Top = 76
  ActiveControl = CheckButton
  BorderStyle = bsDialog
  Caption = 'MainForm'
  ClientHeight = 368
  ClientWidth = 490
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ＭＳ Ｐゴシック'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 12
  object NotYetCheckedLabel: TLabel
    Left = 96
    Top = 328
    Width = 51
    Height = 12
    Caption = '未チェック'
    Visible = False
  end
  object CheckingLabel: TLabel
    Left = 152
    Top = 328
    Width = 83
    Height = 12
    Caption = 'チェック中(%d%%)'
    Visible = False
  end
  object BrokenLabel: TLabel
    Left = 272
    Top = 328
    Width = 24
    Height = 12
    Caption = '破損'
    Visible = False
  end
  object NotBrokenLabel: TLabel
    Left = 240
    Top = 328
    Width = 24
    Height = 12
    Caption = '正常'
    Visible = False
  end
  object MoreThanOneFileIsBrokenLabel: TLabel
    Left = 112
    Top = 344
    Width = 148
    Height = 12
    Caption = ' 破損したファイルがあります '
    Color = clRed
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clWhite
    Font.Height = -12
    Font.Name = 'ＭＳ Ｐゴシック'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Visible = False
  end
  object CheckingButtonLabel: TLabel
    Left = 104
    Top = 352
    Width = 57
    Height = 12
    Caption = 'チェック中...'
    Visible = False
  end
  object TheResultWasCopiedIntoClipboardLabel: TLabel
    Left = 304
    Top = 328
    Width = 187
    Height = 12
    Caption = '結果をクリップボードにコピーしました'
    Visible = False
  end
  object NoticeMemo: TMemo
    Left = 8
    Top = 8
    Width = 473
    Height = 57
    BorderStyle = bsNone
    Color = clBtnFace
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'ＭＳ Ｐゴシック'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 0
  end
  object ListView: TListView
    Left = 8
    Top = 72
    Width = 473
    Height = 249
    Columns = <
      item
        Caption = 'ファイル名'
        Width = 180
      end
      item
        Caption = '状態'
        Width = 80
      end>
    ColumnClick = False
    GridLines = True
    RowSelect = True
    ShowWorkAreas = True
    SortType = stText
    TabOrder = 1
    ViewStyle = vsReport
  end
  object CheckButton: TButton
    Left = 8
    Top = 336
    Width = 89
    Height = 25
    Caption = 'チェック(&V)'
    Default = True
    TabOrder = 2
    OnClick = CheckButtonClick
  end
  object CloseButton: TButton
    Left = 392
    Top = 336
    Width = 89
    Height = 25
    Cancel = True
    Caption = '閉じる(&X)'
    TabOrder = 4
    OnClick = CloseButtonClick
  end
  object CopyResultButton: TButton
    Left = 272
    Top = 336
    Width = 107
    Height = 25
    Hint = '結果をクリップボードにコピーします'
    Caption = '結果をコピー(&C)'
    Enabled = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = CopyResultButtonClick
  end
end
