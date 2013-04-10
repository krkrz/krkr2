object LinkDetailForm: TLinkDetailForm
  Left = 43
  Top = 230
  Width = 637
  Height = 350
  ActiveControl = WaveAreaPanel
  Caption = 'リンクの編集'
  Color = clBtnFace
  Constraints.MinHeight = 189
  Constraints.MinWidth = 612
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ＭＳ Ｐゴシック'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnMouseWheel = FormMouseWheel
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 12
  inline EditLinkAttribFrame: TEditLinkAttribFrame
    Width = 629
    Align = alTop
    TabOrder = 3
  end
  object ToolBarPanel: TPanel
    Left = 0
    Top = 252
    Width = 629
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
      Images = TSSLoopTuner2MainForm.ImageList
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
    Top = 283
    Width = 629
    Height = 33
    Align = alBottom
    BevelOuter = bvNone
    BorderWidth = 4
    TabOrder = 1
    object PlayBeforeLabel: TLabel
      Left = 304
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
      Width = 288
      Height = 25
      Align = alLeft
      AutoSize = True
      ButtonWidth = 25
      Caption = 'BottomToolBar'
      EdgeBorders = []
      Flat = True
      Images = TSSLoopTuner2MainForm.ImageList
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
      object SmoothToolButton: TToolButton
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
      object StopPlayToolButton: TToolButton
        Left = 138
        Top = 0
        Action = StopPlayAction
        ParentShowHint = False
        ShowHint = True
      end
      object PlayHalfSecToolButton: TToolButton
        Left = 163
        Top = 0
        Action = PlayHalfSecAction
        ParentShowHint = False
        ShowHint = True
      end
      object Play1SecToolButton: TToolButton
        Left = 188
        Top = 0
        Action = Play1SecAction
        ParentShowHint = False
        ShowHint = True
      end
      object Play2SecToolButton: TToolButton
        Left = 213
        Top = 0
        Action = Play2SecAction
        Marked = True
        ParentShowHint = False
        ShowHint = True
      end
      object Play3SecToolButton: TToolButton
        Left = 238
        Top = 0
        Action = Play3SecAction
        ParentShowHint = False
        ShowHint = True
      end
      object Play5SecToolButton: TToolButton
        Left = 263
        Top = 0
        Action = Play5SecAction
        ParentShowHint = False
        ShowHint = True
      end
    end
    object OKButton: TButton
      Left = 454
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
      Left = 542
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
      Left = 304
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
    Top = 30
    Width = 629
    Height = 222
    Align = alClient
    BevelOuter = bvLowered
    PopupMenu = WaveAreaPopupMenu
    TabOrder = 2
    OnEnter = WaveAreaPanelEnter
    OnExit = WaveAreaPanelExit
    object WavePaintBox: TPaintBox
      Left = 1
      Top = 1
      Width = 627
      Height = 220
      Align = alClient
      OnDblClick = WavePaintBoxDblClick
      OnMouseDown = WavePaintBoxMouseDown
      OnMouseMove = WavePaintBoxMouseMove
      OnMouseUp = WavePaintBoxMouseUp
      OnPaint = WavePaintBoxPaint
    end
    object AfterLinkLabel: TLabel
      Left = 568
      Top = 8
      Width = 45
      Height = 12
      Anchors = [akTop, akRight]
      Caption = 'リンク後'
      Font.Charset = SHIFTJIS_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'ＭＳ Ｐゴシック'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object BeforeLinkLabel: TLabel
      Left = 16
      Top = 8
      Width = 45
      Height = 12
      Caption = 'リンク前'
      Font.Charset = SHIFTJIS_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'ＭＳ Ｐゴシック'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
  end
  object ActionList: TActionList
    Images = TSSLoopTuner2MainForm.ImageList
    Left = 456
    Top = 24
    object StopPlayAction: TAction
      Category = '再生'
      Caption = '再生停止(&Q)'
      Hint = '再生を停止'
      ImageIndex = 4
      ShortCut = 81
      OnExecute = StopPlayActionExecute
    end
    object ZoomInAction: TAction
      Category = '表示'
      Caption = 'ズーム・イン(&I)'
      Hint = '波形を拡大する'
      ImageIndex = 9
      ShortCut = 73
      OnExecute = ZoomInActionExecute
    end
    object ZoomOutAction: TAction
      Category = '表示'
      Caption = 'ズーム・アウト(&O)'
      Hint = '波形を縮小する'
      ImageIndex = 10
      ShortCut = 79
      OnExecute = ZoomOutActionExecute
    end
    object PlayHalfSecAction: TAction
      Category = '再生'
      Caption = '0.5秒前から再生(&0)'
      Hint = '0.5秒前から再生'
      ImageIndex = 25
      ShortCut = 16432
      OnExecute = PlayHalfSecActionExecute
    end
    object Play1SecAction: TAction
      Category = '再生'
      Caption = '1秒前から再生(&1)'
      Hint = '1秒前から再生'
      ImageIndex = 26
      ShortCut = 16433
      OnExecute = Play1SecActionExecute
    end
    object Play2SecAction: TAction
      Category = '再生'
      Caption = '2秒前から再生(&2)'
      Hint = '2秒前から再生'
      ImageIndex = 27
      ShortCut = 16434
      OnExecute = Play2SecActionExecute
    end
    object Play3SecAction: TAction
      Category = '再生'
      Caption = '3秒前から再生(&3)'
      Hint = '3秒前から再生'
      ImageIndex = 28
      ShortCut = 16435
      OnExecute = Play3SecActionExecute
    end
    object Play5SecAction: TAction
      Category = '再生'
      Caption = '5秒前から再生(&5)'
      Hint = '5秒前から再生'
      ImageIndex = 29
      ShortCut = 16437
      OnExecute = Play5SecActionExecute
    end
    object SmoothAction: TAction
      Category = '属性'
      Caption = 'スムーズ(&S)'
      Hint = 'リンクをスムーズにする'
      ImageIndex = 30
      ShortCut = 77
      OnExecute = SmoothActionExecute
    end
    object BeforePrevCrossAction: TAction
      Category = 'リンク前'
      Caption = '前のクロッシング・ポイントへ(&A)'
      Hint = '前のクロッシング・ポイントへ'
      ImageIndex = 22
      ShortCut = 65
      OnExecute = BeforePrevCrossActionExecute
    end
    object BeforePrevFastAction: TAction
      Category = 'リンク前'
      Caption = '前へ20ステップ(&S)'
      Hint = '前へ20ステップ'
      ImageIndex = 23
      ShortCut = 83
      OnExecute = BeforePrevFastActionExecute
    end
    object BeforePrevStepAction: TAction
      Category = 'リンク前'
      Caption = '前へ1ステップ(&D)'
      Hint = '前へ1ステップ'
      ImageIndex = 21
      ShortCut = 68
      OnExecute = BeforePrevStepActionExecute
    end
    object BeforeNextStepAction: TAction
      Category = 'リンク前'
      Caption = '次へ1ステップ(&F)'
      Hint = '次へ1ステップ'
      ImageIndex = 18
      ShortCut = 70
      OnExecute = BeforeNextStepActionExecute
    end
    object BeforeNextFastAction: TAction
      Category = 'リンク前'
      Caption = '次へ20ステップ(&G)'
      Hint = '次へ20ステップ'
      ImageIndex = 20
      ShortCut = 71
      OnExecute = BeforeNextFastActionExecute
    end
    object BeforeNextCrossAction: TAction
      Category = 'リンク前'
      Caption = '次のクロッシング・ポイントへ(&H)'
      Hint = '次のクロッシング・ポイントへ'
      ImageIndex = 19
      ShortCut = 72
      OnExecute = BeforeNextCrossActionExecute
    end
    object AfterPrevCrossAction: TAction
      Category = 'リンク後'
      Caption = '前のクロッシング・ポイントへ(&Z)'
      Hint = '前のクロッシング・ポイントへ'
      ImageIndex = 22
      ShortCut = 90
      OnExecute = AfterPrevCrossActionExecute
    end
    object AfterPrevFastAction: TAction
      Category = 'リンク後'
      Caption = '前へ20ステップ(&X)'
      Hint = '前へ20ステップ'
      ImageIndex = 23
      ShortCut = 88
      OnExecute = AfterPrevFastActionExecute
    end
    object AfterPrevStepAction: TAction
      Category = 'リンク後'
      Caption = '前へ1ステップ(&C)'
      Hint = '前へ1ステップ'
      ImageIndex = 21
      ShortCut = 67
      OnExecute = AfterPrevStepActionExecute
    end
    object AfterNextStepAction: TAction
      Category = 'リンク後'
      Caption = '次へ1ステップ(&V)'
      Hint = '次へ1ステップ'
      ImageIndex = 18
      ShortCut = 86
      OnExecute = AfterNextStepActionExecute
    end
    object AfterNextFastAction: TAction
      Category = 'リンク後'
      Caption = '次へ20ステップ(&B)'
      Hint = '次へ20ステップ'
      ImageIndex = 20
      ShortCut = 66
      OnExecute = AfterNextFastActionExecute
    end
    object AfterNextCrossAction: TAction
      Category = 'リンク後'
      Caption = '次のクロッシング・ポイントへ(&N)'
      Hint = '次のクロッシング・ポイントへ'
      ImageIndex = 19
      ShortCut = 78
      OnExecute = AfterNextCrossActionExecute
    end
    object PlayAction: TAction
      Category = '再生'
      Caption = 'PlayAction'
      ImageIndex = 24
      ShortCut = 32
      OnExecute = PlayActionExecute
    end
  end
  object ApplicationEvents: TApplicationEvents
    OnIdle = ApplicationEventsIdle
    Left = 488
    Top = 24
  end
  object WaveAreaPopupMenu: TPopupMenu
    Images = TSSLoopTuner2MainForm.ImageList
    Left = 520
    Top = 24
    object ZoomInMenuItem: TMenuItem
      Action = ZoomInAction
    end
    object ZoomOutMenuItem: TMenuItem
      Action = ZoomOutAction
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object PlayMenuItem: TMenuItem
      Caption = '再生'
      Default = True
      ImageIndex = 24
      ShortCut = 32
      OnClick = PlayMenuItemClick
    end
    object StopPlayMenuItem: TMenuItem
      Action = StopPlayAction
    end
    object PlayHalfSecMenuItem: TMenuItem
      Action = PlayHalfSecAction
    end
    object Play1SecMenuItem: TMenuItem
      Action = Play1SecAction
    end
    object Play2SecMenuItem: TMenuItem
      Action = Play2SecAction
    end
    object Play3SecMenuItem: TMenuItem
      Action = Play3SecAction
    end
    object Play5SecMenuItem: TMenuItem
      Action = Play5SecAction
    end
    object BeforePrevCrossMenuItem: TMenuItem
      Action = BeforePrevCrossAction
      Break = mbBarBreak
      Caption = ' '
    end
    object BeforePrevFastMenuItem: TMenuItem
      Action = BeforePrevFastAction
      Caption = ' '
    end
    object BeforePrevSepMenuItem: TMenuItem
      Action = BeforePrevStepAction
      Caption = ' '
    end
    object BeforeNextStepMenuItem: TMenuItem
      Action = BeforeNextStepAction
      Caption = ' '
    end
    object BeforeNextFastMenuItem: TMenuItem
      Action = BeforeNextFastAction
      Caption = ' '
    end
    object BeforeNextCrossMenuItem: TMenuItem
      Action = BeforeNextCrossAction
      Caption = ' '
    end
    object AfterPrevCrossMenuItem: TMenuItem
      Action = AfterPrevCrossAction
      Break = mbBarBreak
      Caption = ' '
    end
    object AfterPrevFastMenuItem: TMenuItem
      Action = AfterPrevFastAction
      Caption = ' '
    end
    object AfterPrevSepMenuItem: TMenuItem
      Action = AfterPrevStepAction
      Caption = ' '
    end
    object AfterNextStepMenuItem: TMenuItem
      Action = AfterNextStepAction
      Caption = ' '
    end
    object AfterNextFastMenuItem: TMenuItem
      Action = AfterNextFastAction
      Caption = ' '
    end
    object AfterNextCrossMenuItem: TMenuItem
      Action = AfterNextCrossAction
      Caption = ' '
    end
  end
end
