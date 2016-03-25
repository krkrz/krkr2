#pragma once

/*
 * WRITTEN BY Masahiko Higashiyama in 2010.
 * - MODIFIED BY Miahmie for kirikir2 utf8hack.
 *
 * THIS CODE IS IN PUBLIC DOMAIN.
 * THIS SOFTWARE IS COMPLETELY FREE TO COPY, MODIFY AND/OR RE-DISTRIBUTE.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

inline bool nmh_is_utf8n(const unsigned char *str, int size){
  if(size < 3) return false;
  if(*str == 0xEF && *(str + 1) == 0xBB && *(str + 2) == 0xBF)
    return true;
  return false;
}

inline int nmh_utf8charlen(const unsigned char c)
{
  if(c == 0x00) return -1;
  if(c < 0x80) return 1;
  if(c < 0xC2) return -1;
  if(c < 0xE0) return 2;
  if(c < 0xF0) return 3;
  if(c < 0xF8) return 4;
  if(c < 0xFC) return 5;
  if(c < 0xFE) return 6;
  return -1;
}

inline int nmh_is_utf8(const unsigned char *str, int size){
  int i,j;
  int charlen;
  const unsigned char *p = str;
  bool isutf8n = false;

  if((isutf8n = nmh_is_utf8n(str, size)) != false)
    str += 3;

  for(i = 0; i < size; i++){
    if((charlen = nmh_utf8charlen(*p)) < 0) return 0;
    p++;
    for(j = 0; j < charlen - 1; j++, i++, p++){
      if(*p == 0x00 || (*p & 0xC0) != 0x80) return 0;
    }
  }
  return isutf8n ? -1 : 1;
}

inline bool nmh_is_ascii(const unsigned char *str, int size){
  const unsigned char *p = str;
  int i = 0;
  for(i = 0; i < size; i++, p++){
    if(*p == 0x00 || *p > 0x7F){
      return false;
    }
  }
  return true;
}

inline float nmh_is_euc(const unsigned char *str, int size){
  int i;
  const unsigned char *p = str;
  unsigned char c1, c2, c3;
  float score = 1.0;

  for(i = 0; i < size; i++, p++){
    c1 = *p;
    if(c1 == 0x00){
      return -1.0;
    }else if(c1 <= 0x7F){
      // ASCII
      continue;
    }else if(c1 >= 0xA1 && c1 <= 0xFE && i < size - 1){
      // kanji
      c2 = *(p + 1);
      if(c2 >= 0xA1 && c2 <= 0xFE){
        i++; p++;
      }else{
        return -1.0;
      }
    }else if(c1 == 0x8E && i < size - 1){
      // kana
      c2 = *(p + 1);
      if(c2 >= 0xA1 && c2 <= 0xDF){
        i++; p++; score *= 0.9f;
      }else{
        return -1.0;
      }
    }else if(c1 == 0x8F && i < size - 2){
      // hojo kanji
      c2 = *(p + 1); c3 = *(p + 2);
      if((c2 >= 0xA1 && c2 <= 0xFE)
         && (c3 >= 0xA1 && c3 <= 0xFE)){
        i += 2; p += 2; score *= 0.9f;
      }else{
        return -1.0;
      }
    }
  }

  return score;
}

inline float nmh_is_sjis(const unsigned char *str, int size){
  int i;
  const unsigned char *p = str;
  unsigned char c1, c2;
  float score = 1.0;

  for(i = 0; i < size; i++, p++){
    c1 = *p;
    if(c1 == 0x00){
      return -1.0;
    }else if(c1 <= 0x7F){
      // ASCII
      continue;
    }else if(c1 >= 0xA1 && c1 <= 0xDF){
      // kana
      score *= 0.9f;
      continue;
    }else if(((c1 >= 0x81 && c1 <= 0x9F) ||
              (c1 >= 0xE0 && c1 <= 0xFC))
             && i < size - 1){
      // kanji
      c2 = *(p + 1);
      if((c2 >= 0x40 && c2 <= 0x7E) ||
         (c2 >= 0x80 && c2 <= 0xFC)){
        i++; p++;
      }else{
        return -1.0;
      }
    }else{
      return -1.0;
    }
  }
  return score;
}
