object FontPreviewForm: TFontPreviewForm
  Left = 105
  Top = 114
  Width = 599
  Height = 390
  Caption = 'ƒtƒHƒ“ƒgƒTƒ“ƒvƒ‹'
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '‚l‚r ‚oƒSƒVƒbƒN'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnMouseWheelDown = FormMouseWheelDown
  OnMouseWheelUp = FormMouseWheelUp
  PixelsPerInch = 96
  TextHeight = 12
  object UpperPanel: TPanel
    Left = 0
    Top = 0
    Width = 591
    Height = 20
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object ZoomComboBox: TComboBox
      Left = 0
      Top = 0
      Width = 145
      Height = 20
      Style = csDropDownList
      DropDownCount = 16
      ItemHeight = 12
      TabOrder = 0
      OnChange = ZoomComboBoxChange
      Items.Strings = (
        'Šg‘å—¦ “™”{'
        'Šg‘å—¦ 2”{'
        'Šg‘å—¦ 3”{'
        'Šg‘å—¦ 4”{'
        'Šg‘å—¦ 5”{'
        'Šg‘å—¦ 6”{'
        'Šg‘å—¦ 7”{'
        'Šg‘å—¦ 8”{'
        'Šg‘å—¦ 9”{'
        'Šg‘å—¦ 10”{'
        'Šg‘å—¦ 11”{'
        'Šg‘å—¦ 12”{'
        'Šg‘å—¦ 13”{'
        'Šg‘å—¦ 14”{'
        'Šg‘å—¦ 15”{'
        'Šg‘å—¦ 16”{')
    end
    object CloseButton: TButton
      Left = 176
      Top = 0
      Width = 75
      Height = 20
      Cancel = True
      Caption = '•Â‚¶‚é(&C)'
      TabOrder = 1
      OnClick = CloseButtonClick
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 337
    Width = 591
    Height = 19
    Panels = <
      item
        Width = 120
      end
      item
        Width = 50
      end>
    SimplePanel = False
  end
  object PreviewPanel: TPanel
    Left = 0
    Top = 20
    Width = 591
    Height = 317
    Align = alClient
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 2
  end
end
