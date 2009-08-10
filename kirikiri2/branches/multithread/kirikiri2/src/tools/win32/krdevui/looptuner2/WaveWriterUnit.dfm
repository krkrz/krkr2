object WaveWriterForm: TWaveWriterForm
  Left = 125
  Top = 96
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'èoóÕíÜ ...'
  ClientHeight = 82
  ClientWidth = 418
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ÇlÇr ÇoÉSÉVÉbÉN'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 12
  object Label: TLabel
    Left = 16
    Top = 8
    Width = 36
    Height = 12
    Caption = '         '
  end
  object ProgressBar: TProgressBar
    Left = 16
    Top = 24
    Width = 385
    Height = 17
    Min = 0
    Max = 50
    TabOrder = 0
  end
  object CancelButton: TButton
    Left = 171
    Top = 48
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'ÉLÉÉÉìÉZÉã'
    Enabled = False
    TabOrder = 1
    OnClick = CancelButtonClick
  end
  object Timer: TTimer
    Interval = 200
    OnTimer = TimerTimer
    Left = 320
    Top = 48
  end
end
