/*
Module : QOIPP.h
Purpose: Defines the interface for a set of MSVC C++ functions which encapsulate reading and writing QOI image files
         as described at https://qoiformat.org/
Created: PJN / 23-12-2021

Copyright (c) 2021 - 2022 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise)
when your product is released in binary form. You are allowed to modify the source code in any way you want
except you cannot modify the copyright details at the top of each module. If you want to distribute source
code with your application, then you are only allowed to distribute versions released by the author. This is
to maintain a single distribution point for the source code.

*/


//////////////////// Macros / Includes ////////////////////////////////////////

#pragma once

#ifndef QOIPP_H
#define QOIPP_H

#include <cstdio>
#include <vector>
#include <array>


//////////////////// Implementation ///////////////////////////////////////////

namespace QOI
{


struct Desc
{
  UINT nWidth;
  UINT nHeight;
  BYTE nChannels;
  UINT nColorspace;
};

union rgba_t
{
  struct
  {
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a;
  } rgba;
  UINT v;
};

static constexpr BYTE QOI_OP_INDEX{0x00};
static constexpr BYTE QOI_OP_DIFF{0x40};
static constexpr BYTE QOI_OP_LUMA{0x80};
static constexpr BYTE QOI_OP_RUN{0xc0};
static constexpr BYTE QOI_OP_RGB{0xfe};
static constexpr BYTE QOI_OP_RGBA{0xff};
static constexpr BYTE QOI_MASK_2{0xc0};
static constexpr int QOI_HEADER_SIZE{14};
static constexpr UINT QOI_PIXELS_MAX{400000000};

constexpr int QOI_COLOR_HASH(_In_ const rgba_t& C)
{
  return (C.rgba.r*3) + (C.rgba.g*5) + (C.rgba.b*7) + (C.rgba.a*11);
}

constexpr UINT QOI_MAGIC()
{
  return ((static_cast<UINT>('q') << 24) | (static_cast<UINT>('o') << 16) | (static_cast<UINT>('i') <<  8) | static_cast<UINT>('f'));
}

static constexpr std::array<BYTE, 8> padding{0, 0, 0, 0, 0, 0, 0, 1};

#pragma warning(suppress: 26440)
static inline void write_32(_Inout_ std::vector<BYTE>& bytes, _In_ UINT v, _Inout_ int& nOffset)
{
#pragma warning(suppress: 26446 26472)
  bytes[nOffset++] = static_cast<BYTE>((0xff000000 & v) >> 24);
#pragma warning(suppress: 26446 26472)
  bytes[nOffset++] = static_cast<BYTE>((0x00ff0000 & v) >> 16);
#pragma warning(suppress: 26446 26472)
  bytes[nOffset++] = static_cast<BYTE>((0x0000ff00 & v) >> 8);
#pragma warning(suppress: 26446 26472)
  bytes[nOffset++] = static_cast<BYTE>(0x000000ff & v);
}

#pragma warning(suppress: 26429)
static inline UINT read_32(_In_ const BYTE* pBytes, _Inout_ int& nOffset) noexcept
{
#pragma warning(suppress: 26481)
  const UINT a{pBytes[nOffset++]};
#pragma warning(suppress: 26481)
  const UINT b{pBytes[nOffset++]};
#pragma warning(suppress: 26481)
  const UINT c{pBytes[nOffset++]};
#pragma warning(suppress: 26481)
  const UINT d{pBytes[nOffset++]};
  return (a << 24) | (b << 16) | (c << 8) | d;
}

static inline std::vector<BYTE> encode(_In_ const BYTE* byData, _In_ const Desc& desc)
{
  if ((byData == nullptr) || (desc.nWidth == 0) || (desc.nHeight == 0) ||
      (desc.nChannels < 3) || (desc.nChannels > 4) || (desc.nColorspace > 1) || (desc.nHeight >= (QOI_PIXELS_MAX / desc.nWidth)))
    return {};
#pragma warning(suppress: 26472)
  const size_t nMaxSize{(static_cast<size_t>(desc.nWidth) * desc.nHeight * (static_cast<size_t>(desc.nChannels) + 1)) + QOI_HEADER_SIZE + sizeof(padding)};
  std::vector<BYTE> encoded{nMaxSize, std::allocator<BYTE>{}};
  int p{0};
  write_32(encoded, QOI_MAGIC(), p);
  write_32(encoded, desc.nWidth, p);
  write_32(encoded, desc.nHeight, p);
#pragma warning(suppress: 26446)
  encoded[p++] = desc.nChannels;
#pragma warning(suppress: 26446 26472)
  encoded[p++] = static_cast<BYTE>(desc.nColorspace);
#pragma warning(suppress: 26429)
  auto pixels{byData};
  std::array<rgba_t, 64> index{};
  int nRun{0};
  rgba_t px_prev;
  px_prev.rgba.r = 0;
  px_prev.rgba.g = 0;
  px_prev.rgba.b = 0;
  px_prev.rgba.a = 255;
  rgba_t px{px_prev};
  const UINT nPXLen{desc.nWidth * desc.nHeight * desc.nChannels};
  const UINT nPXEnd{nPXLen - desc.nChannels};
  const int nChannels{desc.nChannels};
  for (UINT nPXPos{0}; nPXPos<nPXLen; nPXPos+=nChannels)
  {
#pragma warning(suppress: 26481)
    px.rgba.r = pixels[nPXPos + 0];
#pragma warning(suppress: 26481)
    px.rgba.g = pixels[nPXPos + 1];
#pragma warning(suppress: 26481)
    px.rgba.b = pixels[nPXPos + 2];
    if (nChannels == 4)
    {
#pragma warning(suppress: 26481)
      px.rgba.a = pixels[nPXPos + 3];
    }
    if (px.v == px_prev.v)
    {
      nRun++;
      if ((nRun == 62) || (nPXPos == nPXEnd))
      {
#pragma warning(suppress: 26446 26472)
        encoded[p++] = static_cast<BYTE>(QOI_OP_RUN | (nRun - 1));
        nRun = 0;
      }
    }
    else
    {
      int nIndexPos{0};
      if (nRun > 0)
      {
#pragma warning(suppress: 26446 26472)
        encoded[p++] = static_cast<BYTE>(QOI_OP_RUN | (nRun - 1));
        nRun = 0;
      }
      nIndexPos = QOI_COLOR_HASH(px) % 64;
#pragma warning(suppress: 26446 26482)
      if (index[nIndexPos].v == px.v)
#pragma warning(suppress: 26472)
        encoded[p++] = static_cast<BYTE>(QOI_OP_INDEX | nIndexPos);
      else
      {
#pragma warning(suppress: 26446 26482)
        index[nIndexPos] = px;
        if (px.rgba.a == px_prev.rgba.a)
        {
          signed char vr = px.rgba.r - px_prev.rgba.r;
          signed char vg= px.rgba.g - px_prev.rgba.g;
          signed char vb = px.rgba.b - px_prev.rgba.b;
          signed char vg_r = vr - vg;
          signed char vg_b = vb - vg;
          if ((vr > -3) && (vr < 2) && (vg > -3) && (vg < 2) && (vb > -3) && (vb < 2))
#pragma warning(suppress: 26446)
            encoded[p++] = QOI_OP_DIFF | (vr + 2) << 4 | (vg + 2) << 2 | (vb + 2);
          else if ((vg_r > -9) && (vg_r < 8) && (vg > -33) && (vg < 32) && (vg_b > -9) && (vg_b < 8))
          {
#pragma warning(suppress: 26446)
            encoded[p++] = QOI_OP_LUMA | (vg + 32);
#pragma warning(suppress: 26446)
            encoded[p++] = (vg_r + 8) << 4 | (vg_b + 8);
          }
          else
          {
#pragma warning(suppress: 26446)
            encoded[p++] = QOI_OP_RGB;
#pragma warning(suppress: 26446)
            encoded[p++] = px.rgba.r;
#pragma warning(suppress: 26446)
            encoded[p++] = px.rgba.g;
#pragma warning(suppress: 26446)
            encoded[p++] = px.rgba.b;
          }
        }
        else
        {
#pragma warning(suppress: 26446)
          encoded[p++] = QOI_OP_RGBA;
#pragma warning(suppress: 26446)
          encoded[p++] = px.rgba.r;
#pragma warning(suppress: 26446)
          encoded[p++] = px.rgba.g;
#pragma warning(suppress: 26446)
          encoded[p++] = px.rgba.b;
#pragma warning(suppress: 26446)
          encoded[p++] = px.rgba.a;
#pragma warning(suppress: 26446)
        }
      }
    }
    px_prev = px;
  }
  for (size_t i{0}; i<sizeof(padding); i++)
#pragma warning(suppress: 26446 26482)
    encoded[p++] = padding[i];
  encoded.resize(p);
  return encoded;
}

static inline std::vector<BYTE> decode(_In_reads_bytes_(nSize) const BYTE* byData, _In_ size_t nSize, _In_ int nChannels, _Inout_ Desc& desc)
{
  //Validate our parameters
  if ((byData == nullptr) || ((nChannels != 0) && (nChannels != 3) && (nChannels != 4)) || (nSize < (QOI_HEADER_SIZE + sizeof(padding))))
    return {};

  //Validate the contents of the header
#pragma warning(suppress: 26429)
  auto bytes{byData};
  int nOffset{0};
  const UINT nMagic{read_32(bytes, nOffset)};
  desc.nWidth = read_32(bytes, nOffset);
  desc.nHeight = read_32(bytes, nOffset);
#pragma warning(suppress: 26481)
  desc.nChannels = bytes[nOffset++];
#pragma warning(suppress: 26481)
  desc.nColorspace = bytes[nOffset++];
  if ((desc.nWidth == 0) || (desc.nHeight == 0) || (desc.nChannels < 3) || (desc.nChannels > 4) || (desc.nColorspace > 1) ||
      (nMagic != QOI_MAGIC()) || (desc.nHeight >= (QOI_PIXELS_MAX / desc.nWidth)))
    return {};

  //If the nChannels parameter is provided as zero, then use the channels value read from the format
  if (nChannels == 0)
    nChannels = desc.nChannels;

  //Allocate the pixels array
  const UINT nPXLen{desc.nWidth * desc.nHeight * nChannels};
  std::vector<BYTE> pixels{nPXLen, std::allocator<BYTE>{}};

  //Run the decoding loop
  std::array<rgba_t, 64> index{};
  rgba_t px;
  px.rgba.r = 0;
  px.rgba.g = 0;
  px.rgba.b = 0;
  px.rgba.a = 255;
  const size_t nChunksLen{nSize - sizeof(padding)};
  int nRun{0};
#pragma warning(suppress: 26472)
  size_t p{static_cast<size_t>(nOffset)};
  for (UINT nPXPos{0}; nPXPos<nPXLen; nPXPos+=nChannels)
  {
    if (nRun > 0)
      nRun--;
    else if (p < nChunksLen)
    {
#pragma warning(suppress: 26481)
      const int b1{bytes[p++]};
      if (b1 == QOI_OP_RGB)
      {
#pragma warning(suppress: 26481)
        px.rgba.r = bytes[p++];
#pragma warning(suppress: 26481)
        px.rgba.g = bytes[p++];
#pragma warning(suppress: 26481)
        px.rgba.b = bytes[p++];
      }
      else if (b1 == QOI_OP_RGBA)
      {
#pragma warning(suppress: 26481)
        px.rgba.r = bytes[p++];
#pragma warning(suppress: 26481)
        px.rgba.g = bytes[p++];
#pragma warning(suppress: 26481)
        px.rgba.b = bytes[p++];
#pragma warning(suppress: 26481)
        px.rgba.a = bytes[p++];
      }
      else if ((b1 & QOI_MASK_2) == QOI_OP_INDEX)
#pragma warning(suppress: 26446 26482)
        px = index[b1];
      else if ((b1 & QOI_MASK_2) == QOI_OP_DIFF)
      {
        px.rgba.r += ((b1 >> 4) & 0x03) - 2;
        px.rgba.g += ((b1 >> 2) & 0x03) - 2;
        px.rgba.b += (b1 & 0x03) - 2;
      }
      else if ((b1 & QOI_MASK_2) == QOI_OP_LUMA)
      {
#pragma warning(suppress: 26481)
        const int b2{bytes[p++]};
        const int vg{(b1 & 0x3f) - 32};
#pragma warning(suppress: 26472)
        px.rgba.r += static_cast<BYTE>(vg - 8 + ((b2 >> 4) & 0x0f));
#pragma warning(suppress: 26472)
        px.rgba.g += static_cast<BYTE>(vg);
#pragma warning(suppress: 26472)
        px.rgba.b += static_cast<BYTE>(vg - 8 + (b2 & 0x0f));
      }
      else if ((b1 & QOI_MASK_2) == QOI_OP_RUN)
        nRun = (b1 & 0x3f);
#pragma warning(suppress: 26446 26482)
      index[QOI_COLOR_HASH(px) % 64] = px;
    }
#pragma warning(suppress: 26446)
    pixels[nPXPos] = px.rgba.r;
#pragma warning(suppress: 26446)
    pixels[nPXPos + size_t{1}] = px.rgba.g;
#pragma warning(suppress: 26446)
    pixels[nPXPos + size_t{2}] = px.rgba.b;
    if (nChannels == 4)
    {
#pragma warning(suppress: 26446)
      pixels[nPXPos + size_t{3}] = px.rgba.a;
    }
  }
  return pixels;
}

static size_t write(_In_z_ const TCHAR* pszFilename, _In_ const BYTE* byData, _In_ const Desc& desc) //NOLINT(clang-diagnostic-unused-function)
{
  FILE* f{nullptr};
#pragma warning(suppress: 26486)
  if (_tfopen_s(&f, pszFilename, _T("wb")) != 0)
    return 0;
  auto encoded{encode(byData, desc)};
  if (encoded.size() == 0)
  {
    fclose(f);
    return 0;
  }
#pragma warning(suppress: 6387)
  if (fwrite(encoded.data(), 1, encoded.size(), f) != encoded.size())
  {
    fclose(f);
    return 0;
  }
#pragma warning(suppress: 6387)
  fclose(f);
  return encoded.size();
}

static std::vector<BYTE> read(_In_z_ const TCHAR* pszFilename, _In_ int nChannels, _Inout_ Desc& desc)
{
  FILE* f{nullptr};
#pragma warning(suppress: 26486)
  if (_tfopen_s(&f, pszFilename, _T("rb")) != 0)
    return {};
#pragma warning(suppress: 6387)
  fseek(f, 0, SEEK_END);
#pragma warning(suppress: 6387)
  const auto nSize{_ftelli64(f)};
  if ((nSize <= 0) || (nSize > _I32_MAX)) //Limit our file reading support to 2GB or less
  {
#pragma warning(suppress: 6387)
    fclose(f);
    return {};
  }
  fseek(f, 0, SEEK_SET);
#pragma warning(suppress: 26472)
  std::vector<BYTE> data{static_cast<size_t>(nSize), std::allocator<BYTE>{}};
#pragma warning(suppress: 6387 26472)
  const auto nBytesRead{fread(data.data(), 1, static_cast<size_t>(nSize), f)};
  fclose(f);
  return decode(data.data(), nBytesRead, nChannels, desc);
}


}; //namespace QOI


#endif //#ifndef QOIPP_H
