/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file config.c
 * \brief gNB configuration performed by RRC or as a consequence of RRC procedures
 * \author  Navid Nikaein and Raymond Knopp, WEI-TAI CHEN
 * \date 2010 - 2014, 2018
 * \version 0.1
 * \company Eurecom, NTUST
 * \email: navid.nikaein@eurecom.fr, kroempa@gmail.com
 * @ingroup _mac

 */

#include "COMMON/platform_types.h"
#include "COMMON/platform_constants.h"
#include "common/ran_context.h"

#include "common/utils/LOG/log.h"
#include "common/utils/LOG/vcd_signal_dumper.h"

#include "NR_BCCH-BCH-Message.h"
#include "NR_ServingCellConfigCommon.h"

#include "LAYER2/NR_MAC_gNB/mac_proto.h"
#include "SCHED_NR/phy_frame_config_nr.h"

#include "NR_MIB.h"

extern RAN_CONTEXT_t RC;
//extern int l2_init_gNB(void);
extern void mac_top_init_gNB(void);
extern uint8_t nfapi_mode;
 
uint16_t config_bandwidth(int mu, int nb_rb, int nr_band)
{
  uint16_t bandwidth;

  if (nr_band < 100)  { //FR1
   switch(mu) {
    case 0 :
      switch(nb_rb) {
        case 25 :
          bandwidth  = 5; 
          break;
        case 52 :
          bandwidth  = 10;
          break; 
        case 79 :
          bandwidth  = 15; 
          break;
        case 106 :
          bandwidth  = 20; 
          break;
        case 133 :
          bandwidth  = 25;
          break; 
        case 160 :
          bandwidth  = 30;
          break; 
        case 216 :
          bandwidth  = 40; 
          break;
        case 270 :
          bandwidth  = 50;
          break;
      default:
        AssertFatal(1==0,"Number of DL resource blocks %d undefined for mu %d\n", nb_rb, mu);
      }
      break;
    case 1 :
      switch(nb_rb) {
        case 11 :
          bandwidth = 5; 
          break;
        case 24 :
          bandwidth  = 10;
          break; 
        case 38 :
          bandwidth  = 15; 
          break;
        case 51 :
          bandwidth  = 20; 
          break;
        case 65 :
          bandwidth  = 25;
          break; 
        case 78 :
          bandwidth  = 30;
          break; 
        case 106 :
          bandwidth  = 40; 
          break;
        case 133 :
          bandwidth  = 50;
          break;
        case 162 :
          bandwidth  = 60;
          break;
        case 189 :
          bandwidth  = 70;
          break;
        case 217 :
          bandwidth = 80;
          break;
        case 245 :
          bandwidth  = 90;
          break;
        case 273 :
          bandwidth  = 100;
          break;
      default:
        AssertFatal(1==0,"Number of DL resource blocks %d undefined for mu %d\n", nb_rb, mu);
      }
      break;
    case 2 :
      switch(nb_rb) {
        case 11 :
          bandwidth = 10;
          break; 
        case 18 :
          bandwidth  = 15; 
          break;
        case 24 :
          bandwidth = 20; 
          break;
        case 31 :
          bandwidth  = 25;
          break; 
        case 38 :
          bandwidth  = 30;
          break; 
        case 51 :
          bandwidth  = 40; 
          break;
        case 65 :
          bandwidth  = 50;
          break;
        case 79 :
          bandwidth  = 60;
          break;
        case 93 :
          bandwidth  = 70;
          break;
        case 107 :
          bandwidth  = 80;
          break;
        case 121 :
          bandwidth  = 90;
          break;
        case 135 :
          bandwidth  = 100;
          break;
      default:
        AssertFatal(1==0,"Number of DL resource blocks %d undefined for mu %d\n", nb_rb, mu);
      }
      break;
   default:
     AssertFatal(1==0,"Numerology %d undefined for band %d in FR1\n", mu,nr_band);
   }
  }
  else {
   switch(mu) {
    case 2 :
      switch(nb_rb) {
        case 66 :
          bandwidth = 50; 
          break;
        case 132 :
          bandwidth = 100;
          break; 
        case 264 :
          bandwidth = 200; 
          break;
      default:
        AssertFatal(1==0,"Number of DL resource blocks %d undefined for mu %d\n", nb_rb, mu);
      }
      break;
    case 3 :
      switch(nb_rb) {
        case 32 :
          bandwidth  = 50; 
          break;
        case 66 :
          bandwidth = 100;
          break; 
        case 132 :
          bandwidth = 200; 
          break;
        case 264 :
          bandwidth = 400; 
          break;
      default:
        AssertFatal(1==0,"Number of DL resource blocks %d undefined for mu %d\n", nb_rb, mu);
      }
      break;
    default:
      AssertFatal(1==0,"Numerology %d undefined for band %d in FR1\n", mu,nr_band);
   }
  }

  return (bandwidth);
}


void config_common(int Mod_idP, NR_ServingCellConfigCommon_t *scc) {

  nfapi_nr_config_request_scf_t *cfg = &RC.nrmac[Mod_idP]->config[0];
  RC.nrmac[Mod_idP]->common_channels[0].ServingCellConfigCommon = scc;
  int i;

  // Carrier configuration

  cfg->carrier_config.dl_bandwidth.value = config_bandwidth(scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList.list.array[0]->subcarrierSpacing,
                                                            scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList.list.array[0]->carrierBandwidth,
                                                            *scc->downlinkConfigCommon->frequencyInfoDL->frequencyBandList.list.array[0]);
  cfg->carrier_config.dl_bandwidth.tl.tag   = NFAPI_NR_CONFIG_DL_BANDWIDTH_TAG; //temporary
  cfg->num_tlv++;
  LOG_I(PHY,"%s() dl_BandwidthP:%d\n", __FUNCTION__, cfg->carrier_config.dl_bandwidth.value);

  cfg->carrier_config.dl_frequency.value = from_nrarfcn(*scc->downlinkConfigCommon->frequencyInfoDL->frequencyBandList.list.array[0],
                                                        scc->downlinkConfigCommon->frequencyInfoDL->absoluteFrequencyPointA)/1000; // freq in kHz
  cfg->carrier_config.dl_frequency.tl.tag = NFAPI_NR_CONFIG_DL_FREQUENCY_TAG;
  cfg->num_tlv++;

  for (i=0; i<5; i++) {
    if (i==scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList.list.array[0]->subcarrierSpacing) {
      cfg->carrier_config.dl_grid_size[i].value = scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList.list.array[0]->carrierBandwidth;
      cfg->carrier_config.dl_k0[i].value = scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList.list.array[0]->offsetToCarrier;
      cfg->carrier_config.dl_grid_size[i].tl.tag = NFAPI_NR_CONFIG_DL_GRID_SIZE_TAG;
      cfg->carrier_config.dl_k0[i].tl.tag = NFAPI_NR_CONFIG_DL_K0_TAG;
      cfg->num_tlv++;
      cfg->num_tlv++;
    }
    else {
      cfg->carrier_config.dl_grid_size[i].value = 0;
      cfg->carrier_config.dl_k0[i].value = 0;
    }
  }

  cfg->carrier_config.uplink_bandwidth.value = config_bandwidth(scc->uplinkConfigCommon->frequencyInfoUL->scs_SpecificCarrierList.list.array[0]->subcarrierSpacing,
                                                                scc->uplinkConfigCommon->frequencyInfoUL->scs_SpecificCarrierList.list.array[0]->carrierBandwidth,
                                                                *scc->uplinkConfigCommon->frequencyInfoUL->frequencyBandList->list.array[0]);
  cfg->carrier_config.uplink_bandwidth.tl.tag   = NFAPI_NR_CONFIG_UPLINK_BANDWIDTH_TAG; //temporary
  cfg->num_tlv++;
  LOG_I(PHY,"%s() dl_BandwidthP:%d\n", __FUNCTION__, cfg->carrier_config.uplink_bandwidth.value);

  int UL_pointA;
  if (scc->uplinkConfigCommon->frequencyInfoUL->absoluteFrequencyPointA == NULL)
    UL_pointA = scc->downlinkConfigCommon->frequencyInfoDL->absoluteFrequencyPointA;
  else
    UL_pointA = *scc->uplinkConfigCommon->frequencyInfoUL->absoluteFrequencyPointA; 

  cfg->carrier_config.uplink_frequency.value = from_nrarfcn(*scc->uplinkConfigCommon->frequencyInfoUL->frequencyBandList->list.array[0],
                                                            UL_pointA)/1000; // freq in kHz
  cfg->carrier_config.uplink_frequency.tl.tag = NFAPI_NR_CONFIG_UPLINK_FREQUENCY_TAG;
  cfg->num_tlv++;

  for (i=0; i<5; i++) {
    if (i==scc->uplinkConfigCommon->frequencyInfoUL->scs_SpecificCarrierList.list.array[0]->subcarrierSpacing) {
      cfg->carrier_config.ul_grid_size[i].value = scc->uplinkConfigCommon->frequencyInfoUL->scs_SpecificCarrierList.list.array[0]->carrierBandwidth;
      cfg->carrier_config.ul_k0[i].value = scc->uplinkConfigCommon->frequencyInfoUL->scs_SpecificCarrierList.list.array[0]->offsetToCarrier;
      cfg->carrier_config.ul_grid_size[i].tl.tag = NFAPI_NR_CONFIG_UL_GRID_SIZE_TAG;
      cfg->carrier_config.ul_k0[i].tl.tag = NFAPI_NR_CONFIG_UL_K0_TAG;
      cfg->num_tlv++;
      cfg->num_tlv++;
    }
    else {
      cfg->carrier_config.ul_grid_size[i].value = 0;
      cfg->carrier_config.ul_k0[i].value = 0;
    }
  }

  // Cell configuration
  cfg->cell_config.phy_cell_id.value = *scc->physCellId;
  cfg->cell_config.phy_cell_id.tl.tag = NFAPI_NR_CONFIG_PHY_CELL_ID_TAG;
  cfg->num_tlv++;

  cfg->cell_config.frame_duplex_type.value = 1;
  cfg->cell_config.frame_duplex_type.tl.tag = NFAPI_NR_CONFIG_FRAME_DUPLEX_TYPE_TAG;
  cfg->num_tlv++;


  // SSB configuration
  cfg->ssb_config.ss_pbch_power.value = scc->ss_PBCH_BlockPower;
  cfg->ssb_config.ss_pbch_power.tl.tag = NFAPI_NR_CONFIG_SS_PBCH_POWER_TAG;
  cfg->num_tlv++;

  cfg->ssb_config.scs_common.value = *scc->ssbSubcarrierSpacing;
  cfg->ssb_config.scs_common.tl.tag = NFAPI_NR_CONFIG_SCS_COMMON_TAG;
  cfg->num_tlv++;

  // PRACH configuration

  cfg->prach_config.prach_sequence_length.value = scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->prach_RootSequenceIndex.present;
  cfg->prach_config.prach_sequence_length.tl.tag = NFAPI_NR_CONFIG_PRACH_SEQUENCE_LENGTH_TAG;
  cfg->num_tlv++;  

  if (scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->msg1_SubcarrierSpacing)
    cfg->prach_config.prach_sub_c_spacing.value = *scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->msg1_SubcarrierSpacing;
  else 
    cfg->prach_config.prach_sub_c_spacing.value = scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList.list.array[0]->subcarrierSpacing;
  cfg->prach_config.prach_sub_c_spacing.tl.tag = NFAPI_NR_CONFIG_PRACH_SUB_C_SPACING_TAG;
  cfg->num_tlv++;

  cfg->prach_config.restricted_set_config.value = scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->restrictedSetConfig;
  cfg->prach_config.restricted_set_config.tl.tag = NFAPI_NR_CONFIG_RESTRICTED_SET_CONFIG_TAG;
  cfg->num_tlv++;

  switch (scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->rach_ConfigGeneric.msg1_FDM) {
    case 0 :
      cfg->prach_config.num_prach_fd_occasions.value = 1;
      break;
    case 1 :
      cfg->prach_config.num_prach_fd_occasions.value = 2;
      break;
    case 2 :
      cfg->prach_config.num_prach_fd_occasions.value = 4;
      break;
    case 3 :
      cfg->prach_config.num_prach_fd_occasions.value = 8;
      break;
    default:
      AssertFatal(1==0,"msg1 FDM identifier %ld undefined (0,1,2,3) \n", scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->rach_ConfigGeneric.msg1_FDM);
  } 
  cfg->prach_config.num_prach_fd_occasions.tl.tag = NFAPI_NR_CONFIG_NUM_PRACH_FD_OCCASIONS_TAG;
  cfg->num_tlv++;

  cfg->prach_config.num_prach_fd_occasions_list = (nfapi_nr_num_prach_fd_occasions_t *) malloc(cfg->prach_config.num_prach_fd_occasions.value*sizeof(nfapi_nr_num_prach_fd_occasions_t));
  for (i=0; i<cfg->prach_config.num_prach_fd_occasions.value; i++) {
    cfg->prach_config.num_prach_fd_occasions_list[i].num_prach_fd_occasions = i;
    if (cfg->prach_config.prach_sequence_length.value)
      cfg->prach_config.num_prach_fd_occasions_list[i].prach_root_sequence_index.value = scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->prach_RootSequenceIndex.choice.l139; 
    else
      cfg->prach_config.num_prach_fd_occasions_list[i].prach_root_sequence_index.value = scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->prach_RootSequenceIndex.choice.l839;
    cfg->prach_config.num_prach_fd_occasions_list[i].prach_root_sequence_index.tl.tag = NFAPI_NR_CONFIG_PRACH_ROOT_SEQUENCE_INDEX_TAG;
    cfg->num_tlv++;
    cfg->prach_config.num_prach_fd_occasions_list[i].k1.value = scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->rach_ConfigGeneric.msg1_FrequencyStart;
    cfg->prach_config.num_prach_fd_occasions_list[i].k1.tl.tag = NFAPI_NR_CONFIG_K1_TAG;
    cfg->num_tlv++;
    cfg->prach_config.num_prach_fd_occasions_list[i].prach_zero_corr_conf.value = scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->rach_ConfigGeneric.zeroCorrelationZoneConfig;
    cfg->prach_config.num_prach_fd_occasions_list[i].prach_zero_corr_conf.tl.tag = NFAPI_NR_CONFIG_PRACH_ZERO_CORR_CONF_TAG;
    cfg->num_tlv++;
    //cfg->prach_config.num_prach_fd_occasions_list[i].num_unused_root_sequences.value = ???
  }

  cfg->prach_config.ssb_per_rach.value = scc->uplinkConfigCommon->initialUplinkBWP->rach_ConfigCommon->choice.setup->ssb_perRACH_OccasionAndCB_PreamblesPerSSB->present;
  cfg->prach_config.ssb_per_rach.tl.tag = NFAPI_NR_CONFIG_SSB_PER_RACH_TAG;
  cfg->num_tlv++;
 
  // SSB Table Configuration
  int scs_scaling = 1<<(cfg->ssb_config.scs_common.value);
  uint32_t absolute_diff = (*scc->downlinkConfigCommon->frequencyInfoDL->absoluteFrequencySSB - scc->downlinkConfigCommon->frequencyInfoDL->absoluteFrequencyPointA);
  cfg->ssb_table.ssb_offset_point_a.value = absolute_diff/(12*scs_scaling);
  cfg->ssb_table.ssb_offset_point_a.tl.tag = NFAPI_NR_CONFIG_SSB_OFFSET_POINT_A_TAG;
  cfg->num_tlv++;

  cfg->ssb_table.ssb_period.value = *scc->ssb_periodicityServingCell;
  cfg->ssb_table.ssb_period.tl.tag = NFAPI_NR_CONFIG_SSB_PERIOD_TAG;
  cfg->num_tlv++;

  switch (scc->ssb_PositionsInBurst->present) {
    case 1 :
      cfg->ssb_table.ssb_mask_list[0].ssb_mask.value = scc->ssb_PositionsInBurst->choice.shortBitmap.buf[0];
      cfg->ssb_table.ssb_mask_list[1].ssb_mask.value = 0;
      break;
    case 2 :
      cfg->ssb_table.ssb_mask_list[0].ssb_mask.value = scc->ssb_PositionsInBurst->choice.mediumBitmap.buf[0];
      cfg->ssb_table.ssb_mask_list[1].ssb_mask.value = 0;
      break;
    case 3 :
      cfg->ssb_table.ssb_mask_list[0].ssb_mask.value = 0;
      cfg->ssb_table.ssb_mask_list[1].ssb_mask.value = 0;
      for (i=0; i<4; i++) {
        cfg->ssb_table.ssb_mask_list[0].ssb_mask.value += (scc->ssb_PositionsInBurst->choice.longBitmap.buf[i]<<i*8);
        cfg->ssb_table.ssb_mask_list[1].ssb_mask.value += (scc->ssb_PositionsInBurst->choice.longBitmap.buf[i+4]<<i*8);
      }
      break;
    default:
      AssertFatal(1==0,"SSB bitmap size value %d undefined (allowed values 1,2,3) \n", scc->ssb_PositionsInBurst->present);
  }
  cfg->ssb_table.ssb_mask_list[0].ssb_mask.tl.tag = NFAPI_NR_CONFIG_SSB_MASK_TAG;
  cfg->num_tlv++;

  // TDD Table Configuration
  //cfg->tdd_table.tdd_period.value = scc->tdd_UL_DL_ConfigurationCommon->pattern1.dl_UL_TransmissionPeriodicity;
  cfg->tdd_table.tdd_period.tl.tag = NFAPI_NR_CONFIG_TDD_PERIOD_TAG;
  cfg->num_tlv++;
  cfg->tdd_table.tdd_period.value = scc->tdd_UL_DL_ConfigurationCommon->pattern1.dl_UL_TransmissionPeriodicity;
  
  if(cfg->cell_config.frame_duplex_type.value == TDD){
  int return_tdd = set_tdd_config_nr(cfg,
		    scc->uplinkConfigCommon->frequencyInfoUL->scs_SpecificCarrierList.list.array[0]->subcarrierSpacing,
                    5000,
                    scc->tdd_UL_DL_ConfigurationCommon->pattern1.nrofDownlinkSlots,
                    scc->tdd_UL_DL_ConfigurationCommon->pattern1.nrofDownlinkSymbols,
                    scc->tdd_UL_DL_ConfigurationCommon->pattern1.nrofUplinkSlots,
                    scc->tdd_UL_DL_ConfigurationCommon->pattern1.nrofUplinkSymbols
                  );

  if (return_tdd !=0){
     LOG_E(PHY,"TDD configuration can not be done\n");
  }
  else LOG_I(PHY,"TDD has been properly configurated\n");
  }


/*
  // PDCCH-ConfigCommon
  cfg->pdcch_config.controlResourceSetZero.value = scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->controlResourceSetZero;
  cfg->pdcch_config.searchSpaceZero.value = scc->downlinkConfigCommon->initialDownlinkBWP->pdcch_ConfigCommon->choice.setup->searchSpaceZero;

  // PDSCH-ConfigCommon
  cfg->pdsch_config.num_PDSCHTimeDomainResourceAllocations.value = scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list.count;
  cfg->pdsch_config.dmrs_TypeA_Position.value = scc->dmrs_TypeA_Position;
  AssertFatal(cfg->pdsch_config.num_PDSCHTimeDomainResourceAllocations.value<=NFAPI_NR_PDSCH_CONFIG_MAXALLOCATIONS,"illegal TimeDomainAllocation count %d\n",cfg->pdsch_config.num_PDSCHTimeDomainResourceAllocations.value);
  for (int i=0;i<cfg->pdsch_config.num_PDSCHTimeDomainResourceAllocations.value;i++) {
    cfg->pdsch_config.PDSCHTimeDomainResourceAllocation_k0[i].value=*scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list.array[i]->k0;
    cfg->pdsch_config.PDSCHTimeDomainResourceAllocation_mappingType[i].value=scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list.array[i]->mappingType;
    cfg->pdsch_config.PDSCHTimeDomainResourceAllocation_startSymbolAndLength[i].value=scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list.array[i]->startSymbolAndLength;
  }

  // PUSCH-ConfigCommon
  cfg->pusch_config.num_PUSCHTimeDomainResourceAllocations.value = scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list.count;
  cfg->pusch_config.dmrs_TypeA_Position.value = scc->dmrs_TypeA_Position+2;
  AssertFatal(cfg->pusch_config.num_PUSCHTimeDomainResourceAllocations.value<=NFAPI_NR_PUSCH_CONFIG_MAXALLOCATIONS,"illegal TimeDomainAllocation count %d\n",cfg->pusch_config.num_PUSCHTimeDomainResourceAllocations.value);
  for (int i=0;i<cfg->pusch_config.num_PUSCHTimeDomainResourceAllocations.value;i++) {
    cfg->pusch_config.PUSCHTimeDomainResourceAllocation_k2[i].value=*scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list.array[0]->k2;
  }*/


}

/*void config_servingcellconfigcommon(){

}*/

int rrc_mac_config_req_gNB(module_id_t Mod_idP, 
			   int ssb_SubcarrierOffset,
                           NR_ServingCellConfigCommon_t *scc,
			   int add_ue,
			   uint32_t rnti,
			   NR_CellGroupConfig_t *secondaryCellGroup
                           ){

  if (scc != NULL ) {
    AssertFatal(scc->ssb_PositionsInBurst->present == NR_ServingCellConfigCommon__ssb_PositionsInBurst_PR_mediumBitmap, "SSB Bitmap is not 8-bits!\n");

    LOG_I(MAC,"Configuring common parameters from NR ServingCellConfig\n");

    config_common(Mod_idP, 
		  scc);
    LOG_E(MAC, "%s() %s:%d RC.nrmac[Mod_idP]->if_inst->NR_PHY_config_req:%p\n", __FUNCTION__, __FILE__, __LINE__, RC.nrmac[Mod_idP]->if_inst->NR_PHY_config_req);
  
    // if in nFAPI mode 
    if ( (nfapi_mode == 1 || nfapi_mode == 2) && (RC.nrmac[Mod_idP]->if_inst->NR_PHY_config_req == NULL) ){
      while(RC.nrmac[Mod_idP]->if_inst->NR_PHY_config_req == NULL) {
	// DJP AssertFatal(RC.nrmac[Mod_idP]->if_inst->PHY_config_req != NULL,"if_inst->phy_config_request is null\n");
	usleep(100 * 1000);
	printf("Waiting for PHY_config_req\n");
      }
    }
  
  
    NR_PHY_Config_t phycfg;
    phycfg.Mod_id = Mod_idP;
    phycfg.CC_id  = 0;
    phycfg.cfg    = &RC.nrmac[Mod_idP]->config[0];

    phycfg.cfg->ssb_table.ssb_subcarrier_offset.value = ssb_SubcarrierOffset;
    phycfg.cfg->ssb_table.ssb_subcarrier_offset.tl.tag = NFAPI_NR_CONFIG_SSB_SUBCARRIER_OFFSET_TAG;
    phycfg.cfg->num_tlv++;

    if (RC.nrmac[Mod_idP]->if_inst->NR_PHY_config_req) RC.nrmac[Mod_idP]->if_inst->NR_PHY_config_req(&phycfg); 
  }
  
  if (secondaryCellGroup) {

    NR_UE_list_t *UE_list = &RC.nrmac[Mod_idP]->UE_list;
    int UE_id;
    if (add_ue == 1) {
      UE_id = add_new_nr_ue(Mod_idP,rnti);
      UE_list->secondaryCellGroup[UE_id] = secondaryCellGroup;
      LOG_I(PHY,"Added new UE_id %d/%x with initial secondaryCellGroup\n",UE_id,rnti);
    }
    else { // secondaryCellGroup has been updated
      UE_id = find_nr_UE_id(Mod_idP,rnti);
      UE_list->secondaryCellGroup[UE_id] = secondaryCellGroup;
      LOG_I(PHY,"Modified UE_id %d/%x with secondaryCellGroup\n",UE_id,rnti);
    }
  }
  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_MAC_CONFIG, VCD_FUNCTION_OUT);
  
    
  return(0);

}// END rrc_mac_config_req_gNB
