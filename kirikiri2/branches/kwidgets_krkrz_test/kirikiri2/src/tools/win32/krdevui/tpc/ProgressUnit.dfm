object ProgressForm: TProgressForm
  Left = 195
  Top = 346
  ActiveControl = CancelButton
  BorderStyle = bsDialog
  Caption = '•ÏŠ·’† ...'
  ClientHeight = 98
  ClientWidth = 261
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '‚l‚r ‚oƒSƒVƒbƒN'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 12
  object CurrentFileLabel: TLabel
    Left = 8
    Top = 8
    Width = 4
    Height = 12
  end
  object ProgressBar: TProgressBar
    Left = 8
    Top = 32
    Width = 241
    Height = 16
    Min = 0
    Max = 100
    TabOrder = 0
  end
  object CancelButton: TButton
    Left = 93
    Top = 64
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'ƒLƒƒƒ“ƒZƒ‹'
    TabOrder = 1
    OnClick = CancelButtonClick
  end
  object Timer: TTimer
    Interval = 100
    OnTimer = TimerTimer
    Left = 216
    Top = 64
  end
end
