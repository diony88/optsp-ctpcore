#include "quotespi.h"
#include <sstream>
#include "../include/quotestruct.hpp"
#include "../include/arch.h"

QuoteSpi::QuoteSpi(Logger * logger, ctpspi_callback_fn qfn)
    : logger(logger), quote_callback(qfn)
{
    std::stringstream log;
    spiData = new CbQuoteField;
    log << "QuoteSpi Inited";
    LOGINFO(logger, log);
}


void QuoteSpi::OnFrontConnected()
{
    std::stringstream log;
    (*quote_callback)(CB_QUOTE_FRONT_CONNECTED, nullptr);
    log << "Connected with Quote Front Server";
    LOGINFO(logger, log);
}


void QuoteSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::stringstream log;
    if (!(pRspInfo && pRspInfo->ErrorID != 0) && pRspUserLogin) {
        CbQuoteRspUserLoginField *data = &spiData->RspUserLogin;
        data->DateStamp = arch_Str2TimeStamp(pRspUserLogin->TradingDay, "00:00:00", 0);
        data->LoginTime = arch_Str2TimeStamp(pRspUserLogin->TradingDay, pRspUserLogin->LoginTime, 0);
        arch_Strcpy(data->BrokerID, pRspUserLogin->BrokerID, sizeof(data->BrokerID));
        arch_Strcpy(data->UserID, pRspUserLogin->UserID, sizeof(data->UserID));
        data->FrontID = pRspUserLogin->FrontID;
        data->SessionID = pRspUserLogin->SessionID;
        data->MaxOrderRef = atol(pRspUserLogin->MaxOrderRef);
        data->SHFETime = arch_Str2TimeStamp(pRspUserLogin->TradingDay, pRspUserLogin->SHFETime, 0);
        data->DCETime = arch_Str2TimeStamp(pRspUserLogin->TradingDay, pRspUserLogin->DCETime, 0);
        data->CZCETime = arch_Str2TimeStamp(pRspUserLogin->TradingDay, pRspUserLogin->CZCETime, 0);
        data->FFEXTime = arch_Str2TimeStamp(pRspUserLogin->TradingDay, pRspUserLogin->FFEXTime, 0);
        data->INETime = arch_Str2TimeStamp(pRspUserLogin->TradingDay, pRspUserLogin->INETime, 0);
        (*quote_callback)(CB_QUOTE_RSP_USER_LOGIN, spiData);
        log << "Login on Quote Front Server, " << pRspUserLogin->TradingDay << ", " << pRspUserLogin->LoginTime;
        LOGINFO(logger, log);
    }
    else {
        (*quote_callback)(CB_QUOTE_RSP_USER_LOGIN, nullptr);
        RSPINFO_ERROR(log, pRspInfo, "Failed to Login Quote Front Server");
    }
}


void QuoteSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::stringstream log;
    if (!(pRspInfo && pRspInfo->ErrorID != 0) && pSpecificInstrument) {
        CbQuoteRspSubMarketDataField *data = &spiData->RspSubMarketData;
        arch_Strcpy(data->InstrumentID, pSpecificInstrument->InstrumentID, sizeof(data->InstrumentID));
        (*quote_callback)(CB_QUOTE_RSP_SUB_MARKET_DATA, spiData);
        log << "Success to Subscribe: " << pSpecificInstrument->InstrumentID;
        LOGDBG(logger, log);
    }
    else {
        (*quote_callback)(CB_QUOTE_RSP_SUB_MARKET_DATA, nullptr);
        RSPINFO_ERROR(log, pRspInfo, "Failed to Subscribe Market Data");
    }
}


void QuoteSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    CbQuoteRtnDepthMarketDataField *handicap = &spiData->RtnDepthMarketData;
    handicap->DateStamp = arch_Str2TimeStamp(pDepthMarketData->TradingDay, pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec);
    handicap->PreClose = pDepthMarketData->PreClosePrice;
    handicap->PreSettlement = pDepthMarketData->PreSettlementPrice;
    handicap->PrePosition = (uint64_t)pDepthMarketData->PreOpenInterest;
    handicap->CurrOpen = pDepthMarketData->OpenPrice;
    handicap->CurrHighest = pDepthMarketData->HighestPrice;
    handicap->CurrLowest = pDepthMarketData->LowestPrice;
    handicap->CurrClose = pDepthMarketData->ClosePrice;
    handicap->CurrSettlement = pDepthMarketData->SettlementPrice;
    handicap->UpperLimit = pDepthMarketData->UpperLimitPrice;
    handicap->LowerLimit = pDepthMarketData->LowerLimitPrice;
    handicap->PreDelta = pDepthMarketData->PreDelta;
    handicap->CurrDelta = pDepthMarketData->CurrDelta;
    handicap->BidPrice[0] = pDepthMarketData->BidPrice1;
    handicap->BidVolume[0] = pDepthMarketData->BidVolume1;
    handicap->AskPrice[0] = pDepthMarketData->AskPrice1;
    handicap->AskVolume[0] = pDepthMarketData->AskVolume1;
    handicap->BidPrice[1] = pDepthMarketData->BidPrice2;
    handicap->BidVolume[1] = pDepthMarketData->BidVolume2;
    handicap->AskPrice[1] = pDepthMarketData->AskPrice2;
    handicap->AskVolume[1] = pDepthMarketData->AskVolume2;
    handicap->BidPrice[2] = pDepthMarketData->BidPrice3;
    handicap->BidVolume[2] = pDepthMarketData->BidVolume3;
    handicap->AskPrice[2] = pDepthMarketData->AskPrice3;
    handicap->AskVolume[2] = pDepthMarketData->AskVolume3;
    handicap->BidPrice[3] = pDepthMarketData->BidPrice4;
    handicap->BidVolume[3] = pDepthMarketData->BidVolume4;
    handicap->AskPrice[3] = pDepthMarketData->AskPrice4;
    handicap->AskVolume[3] = pDepthMarketData->AskVolume4;
    handicap->BidPrice[4] = pDepthMarketData->BidPrice5;
    handicap->BidVolume[4] = pDepthMarketData->BidVolume5;
    handicap->AskPrice[4] = pDepthMarketData->AskPrice5;
    handicap->AskVolume[4] = pDepthMarketData->AskVolume5;
    arch_Strcpy(handicap->InstrumentID, pDepthMarketData->InstrumentID, sizeof(handicap->InstrumentID));
    handicap->Price = pDepthMarketData->LastPrice;
    handicap->Volume = pDepthMarketData->Volume;
    handicap->Position = (long)pDepthMarketData->OpenInterest;
    handicap->Turnover = pDepthMarketData->Turnover;
    handicap->Average = pDepthMarketData->AveragePrice;
    (*quote_callback)(CB_QUOTE_RTN_DEPTH_MARKET_DATA, spiData);
}


void QuoteSpi::OnFrontDisconnected(int nReason)
{
    std::stringstream log;
    (*quote_callback)(CB_QUOTE_FRONT_DISCONNECTED, nullptr);
    log << "nReason=" << nReason;
    LOGERR(logger, log);
}


void QuoteSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::stringstream log;
    RSPINFO_ERROR(log, pRspInfo, "Error");
}


void QuoteSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::stringstream log;
    if (!(pRspInfo && pRspInfo->ErrorID != 0) && pUserLogout) {
        CbQuoteRspUserLogoutField *data = &spiData->RspUserLogout;
        arch_Strcpy(data->BrokerID, pUserLogout->BrokerID, sizeof(data->BrokerID));
        arch_Strcpy(data->UserID, pUserLogout->UserID, sizeof(data->UserID));
        (*quote_callback)(CB_QUOTE_RSP_USER_LOGOUT, spiData);
        log << "Success to Logout";
        LOGINFO(logger, log);
    }
    else {
        (*quote_callback)(CB_QUOTE_RSP_USER_LOGOUT, nullptr);
        RSPINFO_ERROR(log, pRspInfo, "Failed to Logout");
    }
}


void QuoteSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::stringstream log;
    if (!(pRspInfo && pRspInfo->ErrorID != 0) && pSpecificInstrument) {
        CbQuoteRspUnsubMarketDataField *data = &spiData->RspUnsubMarketData;
        arch_Strcpy(data->InstrumentID, pSpecificInstrument->InstrumentID, sizeof(data->InstrumentID));
        (*quote_callback)(CB_QUOTE_RSP_UNSUB_MARKET_DATA, spiData);
        log << "Success to Unsubscribe: " << pSpecificInstrument->InstrumentID;
        LOGDBG(logger, log);
    }
    else {
        (*quote_callback)(CB_QUOTE_RSP_UNSUB_MARKET_DATA, nullptr);
        RSPINFO_ERROR(log, pRspInfo, "Failed to Unsubscribe Market Data");
    }
}
