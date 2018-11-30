#include "bands.h"
#include "functions.h"

const BAND_MAP BANDS[] =
{
	//160METERS
	{
		.name = "160m",
		.startFreq = 1810000,
		.endFreq = 2000000,
		.regions = (const REGION_MAP[])
		{
			{
				.startFreq = 1810000,
				.endFreq = 1838000,
				.mode = TRX_MODE_CW
			},
			{
				.startFreq = 1838000,
				.endFreq = 1843000,
				.mode = TRX_MODE_DIGI_U
			},
			{
				.startFreq = 1843000,
				.endFreq = 2000000,
				.mode = TRX_MODE_LSB
			},
		},
		.regionsCount = 3,
	},
	//80METERS
	{
		.name = "80m",
		.startFreq = 3500000,
		.endFreq = 3800000,
		.regions = (const REGION_MAP[])
		{
			{
				.startFreq = 3500000,
				.endFreq = 3580000,
				.mode = TRX_MODE_CW
			},
			{
				.startFreq = 3580000,
				.endFreq = 3600000,
				.mode = TRX_MODE_DIGI_U
			},
			{
				.startFreq = 3600000,
				.endFreq = 3800000,
				.mode = TRX_MODE_LSB
			},
		},
		.regionsCount = 3,
	},
					//40METERS
					{
						.name = "40m",
						.startFreq = 7000000,
						.endFreq = 7300000,
						.regions = (const REGION_MAP[])
						{
							{
								.startFreq = 7000000,
								.endFreq = 7040000,
								.mode = TRX_MODE_CW
							},
							{
								.startFreq = 7040000,
								.endFreq = 7060000,
								.mode = TRX_MODE_DIGI_U
							},
							{
								.startFreq = 7060000,
								.endFreq = 7074000,
								.mode = TRX_MODE_LSB
							},
							{
								.startFreq = 7074000,
								.endFreq = 7080000,
								.mode = TRX_MODE_DIGI_U
							},
							{
								.startFreq = 7080000,
								.endFreq = 7200000,
								.mode = TRX_MODE_LSB
							},
							{
								.startFreq = 7200000,
								.endFreq = 7300000,
								.mode = TRX_MODE_AM
							},
						},
						.regionsCount = 6,
					},
					//30METERS
					{
						.name = "30m",
						.startFreq = 10100000,
						.endFreq = 10150000,
						.regions = (const REGION_MAP[])
						{
							{
								.startFreq = 10100000,
								.endFreq = 10140000,
								.mode = TRX_MODE_CW
							},
							{
								.startFreq = 10140000,
								.endFreq = 10150000,
								.mode = TRX_MODE_DIGI_U
							},
						},
						.regionsCount = 2,
					},
									//20METERS
									{
										.name = "20m",
										.startFreq = 14000000,
										.endFreq = 14350000,
										.regions = (const REGION_MAP[])
										{
											{
												.startFreq = 14000000,
												.endFreq = 14070000,
												.mode = TRX_MODE_CW
											},
											{
												.startFreq = 14070000,
												.endFreq = 14099000,
												.mode = TRX_MODE_DIGI_U
											},
											{
												.startFreq = 14099000,
												.endFreq = 14101000,
												.mode = TRX_MODE_NO_TX
											},
											{
												.startFreq = 14101000,
												.endFreq = 14112000,
												.mode = TRX_MODE_DIGI_U
											},
											{
												.startFreq = 14112000,
												.endFreq = 14350000,
												.mode = TRX_MODE_USB
											},
										},
										.regionsCount = 5,
									},
									//17METERS
									{
										.name = "17m",
										.startFreq = 18068000,
										.endFreq = 18168000,
										.regions = (const REGION_MAP[])
										{
											{
												.startFreq = 18068000,
												.endFreq = 18095000,
												.mode = TRX_MODE_CW
											},
											{
												.startFreq = 18095000,
												.endFreq = 18109000,
												.mode = TRX_MODE_DIGI_U
											},
											{
												.startFreq = 18109000,
												.endFreq = 18111000,
												.mode = TRX_MODE_NO_TX
											},
											{
												.startFreq = 18111000,
												.endFreq = 18168000,
												.mode = TRX_MODE_USB
											},
										},
										.regionsCount = 4,
									},
													//15METERS
													{
														.name = "15m",
														.startFreq = 21000000,
														.endFreq = 21450000,
														.regions = (const REGION_MAP[])
														{
															{
																.startFreq = 21000000,
																.endFreq = 21070000,
																.mode = TRX_MODE_CW
															},
															{
																.startFreq = 21070000,
																.endFreq = 21149000,
																.mode = TRX_MODE_DIGI_U
															},
															{
																.startFreq = 21149000,
																.endFreq = 21151000,
																.mode = TRX_MODE_NO_TX
															},
															{
																.startFreq = 21151000,
																.endFreq = 21450000,
																.mode = TRX_MODE_USB
															},
														},
														.regionsCount = 4,
													},
													//12METERS
													{
														.name = "12m",
														.startFreq = 24890000,
														.endFreq = 24990000,
														.regions = (const REGION_MAP[])
														{
															{
																.startFreq = 24890000,
																.endFreq = 24915000,
																.mode = TRX_MODE_CW
															},
															{
																.startFreq = 24915000,
																.endFreq = 24929000,
																.mode = TRX_MODE_DIGI_U
															},
															{
																.startFreq = 24929000,
																.endFreq = 24931000,
																.mode = TRX_MODE_NO_TX
															},
															{
																.startFreq = 24931000,
																.endFreq = 24940000,
																.mode = TRX_MODE_DIGI_U
															},
															{
																.startFreq = 24940000,
																.endFreq = 24990000,
																.mode = TRX_MODE_USB
															},
														},
														.regionsCount = 5,
													},
																	//10METERS
																	{
																		.name = "10m",
																		.startFreq = 28000000,
																		.endFreq = 29700000,
																		.regions = (const REGION_MAP[])
																		{
																			{
																				.startFreq = 28000000,
																				.endFreq = 28070000,
																				.mode = TRX_MODE_CW
																			},
																			{
																				.startFreq = 28070000,
																				.endFreq = 28190000,
																				.mode = TRX_MODE_DIGI_U
																			},
																			{
																				.startFreq = 28190000,
																				.endFreq = 28199000,
																				.mode = TRX_MODE_CW
																			},
																			{
																				.startFreq = 28199000,
																				.endFreq = 28201000,
																				.mode = TRX_MODE_NO_TX
																			},
																			{
																				.startFreq = 28201000,
																				.endFreq = 28300000,
																				.mode = TRX_MODE_USB
																			},
																			{
																				.startFreq = 28300000,
																				.endFreq = 28320000,
																				.mode = TRX_MODE_DIGI_U
																			},
																			{
																				.startFreq = 28320000,
																				.endFreq = 29200000,
																				.mode = TRX_MODE_USB
																			},
																			{
																				.startFreq = 29200000,
																				.endFreq = 29300000,
																				.mode = TRX_MODE_DIGI_U
																			},
																			{
																				.startFreq = 29300000,
																				.endFreq = 29510000,
																				.mode = TRX_MODE_USB
																			},
																			{
																				.startFreq = 29510000,
																				.endFreq = 29520000,
																				.mode = TRX_MODE_NO_TX
																			},
																			{
																				.startFreq = 29520000,
																				.endFreq = 29700000,
																				.mode = TRX_MODE_FM
																			},
																		},
																		.regionsCount = 11,
																	},
																	//
};

uint8_t getModeFromFreq(uint32_t freq)
{
	uint8_t ret = 0;
	if (freq < 10000000) ret = TRX_MODE_LSB;
	if (freq > 10000000) ret = TRX_MODE_USB;
	if (freq > 30000000) ret = TRX_MODE_FM;
	for (int b = 0; b < BANDS_COUNT; b++)
	{
		if (BANDS[b].startFreq <= freq && freq <= BANDS[b].endFreq)
			for (int r = 0; r < BANDS[b].regionsCount; r++)
			{
				if (BANDS[b].regions[r].startFreq <= freq && freq < BANDS[b].regions[r].endFreq)
				{
					ret = BANDS[b].regions[r].mode;
				}
			}
	}
	return ret;
}
