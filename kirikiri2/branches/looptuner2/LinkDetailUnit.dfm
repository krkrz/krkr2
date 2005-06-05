object LinkDetailForm: TLinkDetailForm
  Left = 70
  Top = 361
  Width = 641
  Height = 438
  Caption = 'LinkDetailForm'
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ＭＳ Ｐゴシック'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 12
  object ToolBarPanel: TPanel
    Left = 0
    Top = 340
    Width = 633
    Height = 31
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object PosAdjustToolBar: TToolBar
      Left = 175
      Top = 4
      Width = 284
      Height = 22
      Align = alNone
      AutoSize = True
      Caption = 'PosAdjustToolBar'
      EdgeBorders = []
      Flat = True
      Images = LoopTunerMainForm.ImageList
      TabOrder = 0
      object BeforePrevCrossToolButton: TToolButton
        Left = 0
        Top = 0
        Action = BeforePrevCrossAction
        ParentShowHint = False
        ShowHint = True
      end
      object BeforePrevFastToolButton: TToolButton
        Left = 23
        Top = 0
        Action = BeforePrevFastAction
        ParentShowHint = False
        ShowHint = True
      end
      object BeforePrevStepToolButton: TToolButton
        Left = 46
        Top = 0
        Action = BeforePrevStepAction
        ParentShowHint = False
        ShowHint = True
      end
      object BeforeNextStepToolButton: TToolButton
        Left = 69
        Top = 0
        Action = BeforeNextStepAction
        ParentShowHint = False
        ShowHint = True
      end
      object BeforeNextFastToolButton: TToolButton
        Left = 92
        Top = 0
        Action = BeforeNextFastAction
        ParentShowHint = False
        ShowHint = True
      end
      object BeforeNextCrossToolButton: TToolButton
        Left = 115
        Top = 0
        Action = BeforeNextCrossAction
        ParentShowHint = False
        ShowHint = True
      end
      object ToolButton2: TToolButton
        Left = 138
        Top = 0
        Width = 8
        Caption = 'ToolButton2'
        ImageIndex = 20
        Style = tbsSeparator
      end
      object AfterPrevCrossToolButton: TToolButton
        Left = 146
        Top = 0
        Action = AfterPrevCrossAction
        ParentShowHint = False
        ShowHint = True
      end
      object AfterPrevFastToolButton: TToolButton
        Left = 169
        Top = 0
        Action = AfterPrevFastAction
        ParentShowHint = False
        ShowHint = True
      end
      object AfterPrevStepToolButton: TToolButton
        Left = 192
        Top = 0
        Action = AfterPrevStepAction
        ParentShowHint = False
        ShowHint = True
      end
      object AfterNextStepToolButton: TToolButton
        Left = 215
        Top = 0
        Action = AfterNextStepAction
        ParentShowHint = False
        ShowHint = True
      end
      object AfterNextFastToolButton: TToolButton
        Left = 238
        Top = 0
        Action = AfterNextFastAction
        ParentShowHint = False
        ShowHint = True
      end
      object AfterNextCrossToolButton: TToolButton
        Left = 261
        Top = 0
        Action = AfterNextCrossAction
        ParentShowHint = False
        ShowHint = True
      end
    end
  end
  object BottomPanel: TPanel
    Left = 0
    Top = 371
    Width = 633
    Height = 33
    Align = alBottom
    BevelOuter = bvNone
    BorderWidth = 4
    TabOrder = 1
    object PlayBeforeLabel: TLabel
      Left = 272
      Top = 5
      Width = 109
      Height = 12
      Hint = 'リンクまであと %.1f 秒'
      Caption = 'リンクまであと %.1f 秒'
      Visible = False
    end
    object BottomToolBar: TToolBar
      Left = 4
      Top = 4
      Width = 263
      Height = 25
      Align = alLeft
      AutoSize = True
      ButtonWidth = 25
      Caption = 'BottomToolBar'
      EdgeBorders = []
      Flat = True
      Images = LoopTunerMainForm.ImageList
      TabOrder = 0
      object ZoomInToolButton: TToolButton
        Left = 0
        Top = 0
        Action = ZoomInAction
        ParentShowHint = False
        ShowHint = True
      end
      object ZoomOutToolButton: TToolButton
        Left = 25
        Top = 0
        Action = ZoomOutAction
        ParentShowHint = False
        ShowHint = True
      end
      object MagnifyLabel: TLabel
        Left = 50
        Top = 0
        Width = 47
        Height = 22
        Alignment = taCenter
        AutoSize = False
        Caption = '/1024'
        Layout = tlCenter
      end
      object ToolButton17: TToolButton
        Left = 97
        Top = 0
        Width = 8
        Caption = 'ToolButton17'
        ImageIndex = 13
        Style = tbsSeparator
      end
      object ToolButton1: TToolButton
        Left = 105
        Top = 0
        Action = SmoothAction
        ParentShowHint = False
        ShowHint = True
      end
      object ToolButton15: TToolButton
        Left = 130
        Top = 0
        Width = 8
        Caption = 'ToolButton15'
        ImageIndex = 25
        Style = tbsSeparator
      end
      object PlayHalfSecToolButton: TToolButton
        Left = 138
        Top = 0
        Action = PlayHalfSecAction
        ParentShowHint = False
        ShowHint = True
      end
      object Play1SecToolButton: TToolButton
        Left = 163
        Top = 0
        Action = Play1SecAction
        ParentShowHint = False
        ShowHint = True
      end
      object Play2SecToolButton: TToolButton
        Left = 188
        Top = 0
        Action = Play2SecAction
        Marked = True
        ParentShowHint = False
        ShowHint = True
      end
      object Play3SecToolButton: TToolButton
        Left = 213
        Top = 0
        Action = Play3SecAction
        ParentShowHint = False
        ShowHint = True
      end
      object Play5SecToolButton: TToolButton
        Left = 238
        Top = 0
        Action = Play5SecAction
        ParentShowHint = False
        ShowHint = True
      end
    end
    object OKButton: TButton
      Left = 458
      Top = 2
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      TabOrder = 2
      OnClick = OKButtonClick
    end
    object CancelButton: TButton
      Left = 546
      Top = 2
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Cancel = True
      Caption = 'キャンセル'
      TabOrder = 3
      OnClick = CancelButtonClick
    end
    object PlayBeforePaintBoxPanel: TPanel
      Left = 272
      Top = 18
      Width = 105
      Height = 7
      BevelOuter = bvLowered
      TabOrder = 1
      object PlayBeforePaintBox: TPaintBox
        Left = 1
        Top = 1
        Width = 103
        Height = 5
        Align = alClient
        OnPaint = PlayBeforePaintBoxPaint
      end
    end
  end
  object WaveAreaPanel: TPanel
    Left = 0
    Top = 0
    Width = 633
    Height = 340
    Align = alClient
    BevelOuter = bvLowered
    TabOrder = 2
    object WavePaintBox: TPaintBox
      Left = 1
      Top = 1
      Width = 631
      Height = 338
      Align = alClient
      OnMouseDown = WavePaintBoxMouseDown
      OnMouseMove = WavePaintBoxMouseMove
      OnMouseUp = WavePaintBoxMouseUp
      OnPaint = WavePaintBoxPaint
    end
  end
  object ActionList: TActionList
    Images = LoopTunerMainForm.ImageList
    Left = 304
    Top = 248
    object ZoomInAction: TAction
      Category = '表示'
      Caption = 'ズーム・イン(&I)'
      Hint = '波形を拡大する'
      ImageIndex = 9
      OnExecute = ZoomInActionExecute
    end
    object ZoomOutAction: TAction
      Category = '表示'
      Caption = 'ズーム・アウト(&O)'
      Hint = '波形を縮小する'
      ImageIndex = 10
      OnExecute = ZoomOutActionExecute
    end
    object PlayHalfSecAction: TAction
      Category = '再生'
      Caption = '0.5秒前から再生(&H)'
      Hint = '0.5秒前から再生'
      ImageIndex = 25
      OnExecute = PlayHalfSecActionExecute
    end
    object Play1SecAction: TAction
      Category = '再生'
      Caption = '1秒前から再生(&1)'
      Hint = '1秒前から再生'
      ImageIndex = 26
      OnExecute = Play1SecActionExecute
    end
    object Play2SecAction: TAction
      Category = '再生'
      Caption = '2秒前から再生(&2)'
      Hint = '2秒前から再生'
      ImageIndex = 27
      OnExecute = Play2SecActionExecute
    end
    object Play3SecAction: TAction
      Category = '再生'
      Caption = '3秒前から再生(&3)'
      Hint = '3秒前から再生'
      ImageIndex = 28
      OnExecute = Play3SecActionExecute
    end
    object Play5SecAction: TAction
      Category = '再生'
      Caption = '5秒前から再生(&5)'
      Hint = '5秒前から再生'
      ImageIndex = 29
      OnExecute = Play5SecActionExecute
    end
    object SmoothAction: TAction
      Category = '属性'
      Caption = 'スムーズ(&S)'
      Hint = 'リンクをスムーズにする'
      ImageIndex = 30
      OnExecute = SmoothActionExecute
    end
    object BeforePrevCrossAction: TAction
      Category = 'リンク前'
      Caption = '前のクロッシング・ポイントへ'
      Hint = '前のクロッシング・ポイントへ'
      ImageIndex = 22
      OnExecute = BeforePrevCrossActionExecute
    end
    object BeforePrevFastAction: TAction
      Category = 'リンク前'
      Caption = '前へ20ステップ'
      Hint = '前へ20ステップ'
      ImageIndex = 23
      OnExecute = BeforePrevFastActionExecute
    end
    object BeforePrevStepAction: TAction
      Category = 'リンク前'
      Caption = '前へ1ステップ'
      Hint = '前へ1ステップ'
      ImageIndex = 21
      OnExecute = BeforePrevStepActionExecute
    end
    object BeforeNextStepAction: TAction
      Category = 'リンク前'
      Caption = '次へ1ステップ'
      Hint = '次へ1ステップ'
      ImageIndex = 18
      OnExecute = BeforeNextStepActionExecute
    end
    object BeforeNextFastAction: TAction
      Category = 'リンク前'
      Caption = '次へ20ステップ'
      Hint = '次へ20ステップ'
      ImageIndex = 20
      OnExecute = BeforeNextFastActionExecute
    end
    object BeforeNextCrossAction: TAction
      Category = 'リンク前'
      Caption = '次のクロッシング・ポイントへ'
      Hint = '次のクロッシング・ポイントへ'
      ImageIndex = 19
      OnExecute = BeforeNextCrossActionExecute
    end
    object AfterPrevCrossAction: TAction
      Category = 'リンク後'
      Caption = '前のクロッシング・ポイントへ'
      Hint = '前のクロッシング・ポイントへ'
      ImageIndex = 22
      OnExecute = AfterPrevCrossActionExecute
    end
    object AfterPrevFastAction: TAction
      Category = 'リンク後'
      Caption = '前へ20ステップ'
      Hint = '前へ20ステップ'
      ImageIndex = 23
      OnExecute = AfterPrevFastActionExecute
    end
    object AfterPrevStepAction: TAction
      Category = 'リンク後'
      Caption = '前へ1ステップ'
      Hint = '前へ1ステップ'
      ImageIndex = 21
      OnExecute = AfterPrevStepActionExecute
    end
    object AfterNextStepAction: TAction
      Category = 'リンク後'
      Caption = '次へ1ステップ'
      Hint = '次へ1ステップ'
      ImageIndex = 18
      OnExecute = AfterNextStepActionExecute
    end
    object AfterNextFastAction: TAction
      Category = 'リンク後'
      Caption = '次へ20ステップ'
      Hint = '次へ20ステップ'
      ImageIndex = 20
      OnExecute = AfterNextFastActionExecute
    end
    object AfterNextCrossAction: TAction
      Category = 'リンク後'
      Caption = '次のクロッシング・ポイントへ'
      Hint = '次のクロッシング・ポイントへ'
      ImageIndex = 19
      OnExecute = AfterNextCrossActionExecute
    end
  end
  object ApplicationEvents: TApplicationEvents
    OnIdle = ApplicationEventsIdle
    Left = 304
    Top = 280
  end
end
