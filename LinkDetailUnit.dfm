object LinkDetailForm: TLinkDetailForm
  Left = 63
  Top = 336
  Width = 643
  Height = 435
  Caption = 'LinkDetailForm'
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ÇlÇr ÇoÉSÉVÉbÉN'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 12
  object ToolBarPanel: TPanel
    Left = 0
    Top = 0
    Width = 635
    Height = 25
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object BeforeToolBar: TToolBar
      Left = 0
      Top = 0
      Width = 138
      Height = 25
      Align = alLeft
      AutoSize = True
      Caption = 'BeforeToolBar'
      EdgeBorders = []
      Flat = True
      Images = LoopTunerMainForm.ImageList
      TabOrder = 0
      object BeforePrevCrossToolButton: TToolButton
        Left = 0
        Top = 0
        Caption = 'BeforePrevCrossToolButton'
        ImageIndex = 22
      end
      object BeforePrevFastToolButton: TToolButton
        Left = 23
        Top = 0
        Caption = 'BeforePrevFastToolButton'
        ImageIndex = 23
      end
      object BeforePrevStepToolButton: TToolButton
        Left = 46
        Top = 0
        Caption = 'BeforePrevStepToolButton'
        ImageIndex = 21
      end
      object BeforeNextStepToolButton: TToolButton
        Left = 69
        Top = 0
        Caption = 'BeforeNextStepToolButton'
        ImageIndex = 18
      end
      object BeforeNextFastToolButton: TToolButton
        Left = 92
        Top = 0
        Caption = 'BeforeNextFastToolButton'
        ImageIndex = 20
      end
      object BeforeNextCrossToolButton: TToolButton
        Left = 115
        Top = 0
        Caption = 'BeforeNextCrossToolButton'
        ImageIndex = 19
      end
    end
    object AfterToolBar: TToolBar
      Left = 497
      Top = 0
      Width = 138
      Height = 25
      Align = alRight
      AutoSize = True
      Caption = 'AfterToolBar'
      EdgeBorders = []
      Flat = True
      Images = LoopTunerMainForm.ImageList
      TabOrder = 1
      object AfterPrevCrossToolButton: TToolButton
        Left = 0
        Top = 0
        Caption = 'AfterPrevCrossToolButton'
        ImageIndex = 22
      end
      object AfterPrevFastToolButton: TToolButton
        Left = 23
        Top = 0
        Caption = 'AfterPrevFastToolButton'
        ImageIndex = 23
      end
      object AfterPrevStepToolButton: TToolButton
        Left = 46
        Top = 0
        Caption = 'AfterPrevStepToolButton'
        ImageIndex = 21
      end
      object AfterNextStepToolButton: TToolButton
        Left = 69
        Top = 0
        Caption = 'AfterNextStepToolButton'
        ImageIndex = 18
      end
      object AfterNextFastToolButton: TToolButton
        Left = 92
        Top = 0
        Caption = 'AfterNextFastToolButton'
        ImageIndex = 20
      end
      object AfterNextCrossToolButton: TToolButton
        Left = 115
        Top = 0
        Caption = 'AfterNextCrossToolButton'
        ImageIndex = 19
      end
    end
  end
  object BottomPanel: TPanel
    Left = 0
    Top = 361
    Width = 635
    Height = 40
    Align = alBottom
    BevelOuter = bvNone
    BevelWidth = 2
    TabOrder = 1
    object BottomToolBar: TToolBar
      Left = 0
      Top = 8
      Width = 288
      Height = 22
      Align = alNone
      AutoSize = True
      ButtonWidth = 27
      Caption = 'BottomToolBar'
      EdgeBorders = []
      Flat = True
      Images = LoopTunerMainForm.ImageList
      TabOrder = 0
      object ZoomInToolButton: TToolButton
        Left = 0
        Top = 0
        Caption = 'ZoomInToolButton'
        ImageIndex = 9
      end
      object ZoomOutToolButton: TToolButton
        Left = 27
        Top = 0
        Caption = 'ZoomOutToolButton'
        ImageIndex = 10
      end
      object ToolButton17: TToolButton
        Left = 54
        Top = 0
        Width = 8
        Caption = 'ToolButton17'
        ImageIndex = 13
        Style = tbsSeparator
      end
      object SmoothCheckBox: TCheckBox
        Left = 62
        Top = 0
        Width = 83
        Height = 22
        Caption = 'ÉXÉÄÅ[ÉY'
        TabOrder = 0
      end
      object ToolButton15: TToolButton
        Left = 145
        Top = 0
        Width = 8
        Caption = 'ToolButton15'
        ImageIndex = 25
        Style = tbsSeparator
      end
      object PlayHalfSecToolButton: TToolButton
        Left = 153
        Top = 0
        Caption = 'PlayHalfSecToolButton'
        ImageIndex = 25
      end
      object Play1SecToolButton: TToolButton
        Left = 180
        Top = 0
        Caption = 'Play1SecToolButton'
        ImageIndex = 26
      end
      object Play2SecToolButton: TToolButton
        Left = 207
        Top = 0
        Caption = 'Play2SecToolButton'
        ImageIndex = 27
      end
      object Play3SecToolButton: TToolButton
        Left = 234
        Top = 0
        Caption = 'Play3SecToolButton'
        ImageIndex = 28
      end
      object Play5SecToolButton: TToolButton
        Left = 261
        Top = 0
        Caption = 'Play5SecToolButton'
        ImageIndex = 29
      end
    end
    object OKButton: TButton
      Left = 460
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      TabOrder = 1
    end
    object CancelButton: TButton
      Left = 548
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Cancel = True
      Caption = 'ÉLÉÉÉìÉZÉã'
      TabOrder = 2
    end
  end
  object WaveAreaPanel: TPanel
    Left = 0
    Top = 25
    Width = 635
    Height = 336
    Align = alClient
    BevelOuter = bvLowered
    TabOrder = 2
    object WavePaintBox: TPaintBox
      Left = 1
      Top = 1
      Width = 633
      Height = 334
      Align = alClient
      OnPaint = WavePaintBoxPaint
    end
  end
end
