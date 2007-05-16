	#pragma comment(lib, "strmiids.lib")
	#include "layerExMovie.hpp"

	/**
	 * コンストラクタ
	 */
	layerExMovie::layerExMovie(DispatchT obj) : _pType(obj, TJS_W("type")), layerExBase(obj)
	{
		pAMStream         = NULL;
		pPrimaryVidStream = NULL;
		pDDStream         = NULL;
		pSample           = NULL; 
		pSurface          = NULL;
		loop = false;
		alpha = false;
		movieWidth = 0;
		movieHeight = 0;
	}

	/**
	 * デストラクタ
	 */
	layerExMovie::~layerExMovie()
	{
		stopMovie();
	}

	/**
	 * ムービーファイルを開いて準備する
	 * @param filename ファイル名
	 * @param alpha アルファ指定（半分のサイズでα処理する）
	 */
	void
	layerExMovie::openMovie(const tjs_char* filename, bool alpha)
	{ 
		this->alpha = alpha;
		movieWidth = 0;
		movieHeight = 0;

		// パス検索
		//ttstr path = TVPGetPlacedPath(ttstr(filename));
		// ローカル名に変換
		//TVPGetLocalName(path);

		// ファイルをテンポラリにコピーして対応
		IStream *in;
		if ((in = TVPCreateIStream(filename, GENERIC_READ)) == NULL) {
			ttstr error = filename;
			error += ":ファイルが開けません";
			TVPAddLog(error);
			return;
		}
		tempFile = TVPGetTemporaryName();
		HANDLE hFile;
		if ((hFile = CreateFile(tempFile.c_str(), GENERIC_WRITE, 0, NULL,
								CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL)) == INVALID_HANDLE_VALUE) {
			TVPAddLog("テンポラリファイルが開けません");
			in->Release();
			tempFile = "";
			return;
		}

		// ファイルをコピー
		BYTE buffer[1024*16];
		DWORD size;
		while (in->Read(buffer, sizeof buffer, &size) == S_OK && size > 0) {			
			WriteFile(hFile, buffer, size, &size, NULL);
		}
		CloseHandle(hFile);
		in->Release();
		
		if (SUCCEEDED(CoCreateInstance(CLSID_AMMultiMediaStream,0,1,IID_IAMMultiMediaStream,(void**)&pAMStream))) {
			if (SUCCEEDED(pAMStream->Initialize(STREAMTYPE_READ, 0, NULL)) &&
				SUCCEEDED(pAMStream->AddMediaStream(0, &MSPID_PrimaryVideo, 0, NULL))) {
				if (SUCCEEDED(pAMStream->OpenFile(tempFile.c_str(), AMMSF_NOCLOCK))) {
					if (SUCCEEDED(pAMStream->GetMediaStream(MSPID_PrimaryVideo, &pPrimaryVidStream))) {
						if (SUCCEEDED(pPrimaryVidStream->QueryInterface(IID_IDirectDrawMediaStream,(void**)&pDDStream))) {

							// フォーマットの指定 ARGB32
							DDSURFACEDESC desc;
							ZeroMemory(&desc, sizeof DDSURFACEDESC);
							desc.dwSize = sizeof(DDSURFACEDESC);
							desc.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT;
							desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
							desc.ddpfPixelFormat.dwSize  = sizeof(DDPIXELFORMAT);
							desc.ddpfPixelFormat.dwFlags = DDPF_RGB; 
							desc.ddpfPixelFormat.dwRGBBitCount = 32;
							desc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
							desc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
							desc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
	//						desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
							HRESULT hr;
							if (SUCCEEDED(hr = pDDStream->SetFormat(&desc, NULL))) {
								if (SUCCEEDED(pDDStream->CreateSample(0,0,0,&pSample))) {
									RECT rect; 
									if (SUCCEEDED(pSample->GetSurface(&pSurface,&rect))) {
										movieWidth  = rect.right - rect.left;
										movieHeight = rect.bottom - rect.top;
										if (alpha) {
											movieWidth /= 2;
										}
										// 画像サイズをムービーのサイズにあわせる
										_pWidth.SetValue(movieWidth);
										_pHeight.SetValue(movieHeight);
										_pType.SetValue(alpha ? ltAlpha : ltOpaque);
									} else {
										// サーフェース取得失敗
										pSample->Release();
										pSample = NULL;
										pDDStream->Release();
										pDDStream = NULL;
										pPrimaryVidStream->Release();
										pPrimaryVidStream = NULL;
										pAMStream->Release();
										pAMStream = NULL;
									}
								}
							} else {
								// サンプル作製失敗
								pDDStream->Release();
								pDDStream = NULL;
								pPrimaryVidStream->Release();
								pPrimaryVidStream = NULL;
								pAMStream->Release();
								pAMStream = NULL;
							}
						} else {
							// DirectDraw ストリーム取得失敗
							pPrimaryVidStream->Release();
							pPrimaryVidStream = NULL;
							pAMStream->Release();
							pAMStream = NULL;
						}
					} else {
						// ビデオストリーム取得失敗
						pAMStream->Release();
						pAMStream = NULL;
					}
				} else {
					// ファイルオープンに失敗
					pAMStream->Release();
					pAMStream = NULL;
				}
			} else {
				pAMStream->Release();
				pAMStream = NULL;
			}
		}
	} 

	/**
	 * ムービーの開始
	 */
	void
	layerExMovie::startMovie(bool loop)
	{
		if (pSample) {
			// 再生開始
			this->loop = loop;
			pAMStream->SetState(STREAMSTATE_RUN);
			pSample->Update(SSUPDATE_ASYNC, NULL, NULL, 0);
			start();
		}
	}

	/**
	 * ムービーの停止
	 */
	void
	layerExMovie::stopMovie()
	{
		stop();
		if (pAMStream) {
			pAMStream->SetState(STREAMSTATE_STOP);
			if (pSurface) {
				pSurface->Release();
				pSurface = NULL;
			}
			if (pSample) {
				pSample->Release();
				pSample = NULL;
			}
			if (pDDStream) {
				pDDStream->Release();
				pDDStream = NULL;
			}
			if (pPrimaryVidStream) {
				pPrimaryVidStream->Release();
				pPrimaryVidStream = NULL;
			}
			if (pAMStream) {
				pAMStream->Release();
				pAMStream = NULL;
			}
		}
		DeleteFile(tempFile.c_str());
		tempFile = "";
	}

	void
	layerExMovie::start()
	{
		stop();
		TVPAddContinuousEventHook(this);
	}

	/**
	 * Irrlicht 呼び出し処理停止
	 */
	void
	layerExMovie::stop()
	{
		TVPRemoveContinuousEventHook(this);
	}

	void TJS_INTF_METHOD
	layerExMovie::OnContinuousCallback(tjs_uint64 tick)
	{
		if (pSample) {
			// 更新
			HRESULT hr = pSample->CompletionStatus(0,0);
			if (hr == MS_S_PENDING) {
	//			TVPAddLog("更新待ち");
			} else if (hr == S_OK) {
				// 更新完了
				// サーフェースからレイヤに画面コピー
				reset();
				DDSURFACEDESC  ddsd; 
				ddsd.dwSize=sizeof(DDSURFACEDESC); 
				if (SUCCEEDED(pSurface->Lock( NULL,&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT , NULL))) {
					if (alpha) {
						int w = movieWidth*4;
						for (int y=0; y<ddsd.dwHeight && y<_height; y++) {
							BYTE *dst  = _buffer+(y*_pitch);
							BYTE *src1 = (BYTE*)ddsd.lpSurface+y*ddsd.lPitch;
							BYTE *src2 = src1 + w;
							for (int x=0; x<_width;x++) {
								*dst++ = *src1++;
								*dst++ = *src1++;
								*dst++ = *src1++;
								*dst++ = *src2; src2+=4; src1++;
							}
						}
					} else {
						int w = _width < movieWidth ? _width * 4 : movieWidth * 4;
						for (int y=0; y<ddsd.dwHeight && y<_height; y++) {
							memcpy(_buffer+(y*_pitch), (BYTE*)ddsd.lpSurface+y*ddsd.lPitch, w);
						}
					}
					pSurface->Unlock(NULL); 
				}
				redraw();
				pSample->Update(SSUPDATE_ASYNC, NULL, NULL, 0);
			} else if (hr == MS_S_ENDOFSTREAM) {
				// 更新終了
				if (loop) {
					pAMStream->Seek(0);
					pSample->Update(SSUPDATE_ASYNC, NULL, NULL, 0);
				} else {
					stopMovie();
				}
			} else {
				stopMovie();
			}
		} else {
			stop();
		}
	}
