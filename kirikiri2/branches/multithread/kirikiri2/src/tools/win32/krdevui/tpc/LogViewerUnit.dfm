object LogViewerForm: TLogViewerForm
  Left = 47
  Top = 354
  Width = 463
  Height = 328
  BorderWidth = 10
  Caption = 'ÉçÉO'
  Color = clBtnFace
  Constraints.MinHeight = 100
  Constraints.MinWidth = 200
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ÇlÇr ÇoÉSÉVÉbÉN'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 12
  object Panel1: TPanel
    Left = 0
    Top = 241
    Width = 435
    Height = 33
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    OnResize = Panel1Resize
    object CloseButton: TButton
      Left = 180
      Top = 8
      Width = 75
      Height = 25
      Caption = 'ï¬Ç∂ÇÈ'
      Default = True
      ModalResult = 1
      TabOrder = 0
    end
  end
  object LogEdit: TRichEdit
    Left = 0
    Top = 0
    Width = 435
    Height = 241
    Align = alClient
    MaxLength = 10000000
    PlainText = True
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 1
    WordWrap = False
  end
end
