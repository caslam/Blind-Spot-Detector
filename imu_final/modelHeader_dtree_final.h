


    // !!! This file is generated using emlearn !!!

    #include "eml_trees.h"
    

static const EmlTreesNode testModel_nodes[59] = {
  { 1, 3, 1, 46 },
  { 0, 9, 1, 30 },
  { 0, 0, 1, 17 },
  { 2, -13, 1, 6 },
  { 1, -7, -1, 1 },
  { 2, -22, 1, 2 },
  { 0, -4, -2, -1 },
  { 0, 0, 1, -2 },
  { 0, -2, -2, -1 },
  { 0, -6, 1, 4 },
  { 1, 1, 1, -2 },
  { 0, -7, 1, -2 },
  { 0, -7, -2, -2 },
  { 2, 13, 1, 3 },
  { 2, -11, -2, 1 },
  { 2, 12, -2, -2 },
  { 1, -3, 1, 2 },
  { 0, -3, -2, -2 },
  { 0, -2, -1, -1 },
  { 2, -36, -1, 1 },
  { 2, 0, 1, 7 },
  { 2, -8, 1, 4 },
  { 1, 0, 1, 2 },
  { 2, -18, -1, -2 },
  { 0, 4, -1, -2 },
  { 2, -1, 1, -2 },
  { 2, -2, -2, -2 },
  { 0, 6, 1, -1 },
  { 0, 0, 1, 2 },
  { 0, 0, -1, -1 },
  { 0, 2, -2, -1 },
  { 2, -3, 1, 10 },
  { 2, -23, 1, 6 },
  { 0, 10, -1, 1 },
  { 0, 23, 1, -1 },
  { 0, 15, 1, 2 },
  { 1, -11, -1, -1 },
  { 1, -3, -1, -1 },
  { 1, -5, -1, 1 },
  { 1, -3, -2, 1 },
  { 2, -14, -1, -2 },
  { 2, 33, 1, 4 },
  { 0, 11, 1, -1 },
  { 1, -10, -1, 1 },
  { 2, 22, -1, -1 },
  { 0, 15, -2, -1 },
  { 2, 4, 1, 11 },
  { 1, 17, 1, 9 },
  { 0, -10, -2, 1 },
  { 2, -14, 1, 4 },
  { 1, 4, -1, 1 },
  { 2, -24, 1, -1 },
  { 1, 7, -1, -1 },
  { 1, 6, 1, -2 },
  { 2, -11, -1, 1 },
  { 2, -8, -1, -1 },
  { 1, 35, -2, -2 },
  { 1, 15, 1, -1 },
  { 0, 0, -1, -1 } 
};

static const int32_t testModel_tree_roots[1] = { 0 };

static const uint8_t testModel_leaves[2] = { 1, 0 };

EmlTrees testModel = {
        59,
        (EmlTreesNode *)(testModel_nodes),	  
        1,
        (int32_t *)(testModel_tree_roots),
        2,
        (uint8_t *)(testModel_leaves),
        0,
        3,
        2,
    };

static inline int32_t testModel_tree_0(const int16_t *features, int32_t features_length) {
          if (features[1] < 3) {
              if (features[0] < 9) {
                  if (features[0] < 0) {
                      if (features[2] < -13) {
                          if (features[1] < -7) {
                              return 1;
                          } else {
                              if (features[2] < -22) {
                                  if (features[0] < -4) {
                                      return 0;
                                  } else {
                                      return 1;
                                  }
                              } else {
                                  if (features[0] < 0) {
                                      if (features[0] < -2) {
                                          return 0;
                                      } else {
                                          return 1;
                                      }
                                  } else {
                                      return 0;
                                  }
                              }
                          }
                      } else {
                          if (features[0] < -6) {
                              if (features[1] < 1) {
                                  if (features[0] < -7) {
                                      if (features[0] < -7) {
                                          return 0;
                                      } else {
                                          return 0;
                                      }
                                  } else {
                                      return 0;
                                  }
                              } else {
                                  return 0;
                              }
                          } else {
                              if (features[2] < 13) {
                                  if (features[2] < -11) {
                                      return 0;
                                  } else {
                                      if (features[2] < 12) {
                                          return 0;
                                      } else {
                                          return 0;
                                      }
                                  }
                              } else {
                                  if (features[1] < -3) {
                                      if (features[0] < -3) {
                                          return 0;
                                      } else {
                                          return 0;
                                      }
                                  } else {
                                      if (features[0] < -2) {
                                          return 1;
                                      } else {
                                          return 1;
                                      }
                                  }
                              }
                          }
                      }
                  } else {
                      if (features[2] < -36) {
                          return 1;
                      } else {
                          if (features[2] < 0) {
                              if (features[2] < -8) {
                                  if (features[1] < 0) {
                                      if (features[2] < -18) {
                                          return 1;
                                      } else {
                                          return 0;
                                      }
                                  } else {
                                      if (features[0] < 4) {
                                          return 1;
                                      } else {
                                          return 0;
                                      }
                                  }
                              } else {
                                  if (features[2] < -1) {
                                      if (features[2] < -2) {
                                          return 0;
                                      } else {
                                          return 0;
                                      }
                                  } else {
                                      return 0;
                                  }
                              }
                          } else {
                              if (features[0] < 6) {
                                  if (features[0] < 0) {
                                      if (features[0] < 0) {
                                          return 1;
                                      } else {
                                          return 1;
                                      }
                                  } else {
                                      if (features[0] < 2) {
                                          return 0;
                                      } else {
                                          return 1;
                                      }
                                  }
                              } else {
                                  return 1;
                              }
                          }
                      }
                  }
              } else {
                  if (features[2] < -3) {
                      if (features[2] < -23) {
                          if (features[0] < 10) {
                              return 1;
                          } else {
                              if (features[0] < 23) {
                                  if (features[0] < 15) {
                                      if (features[1] < -11) {
                                          return 1;
                                      } else {
                                          return 1;
                                      }
                                  } else {
                                      if (features[1] < -3) {
                                          return 1;
                                      } else {
                                          return 1;
                                      }
                                  }
                              } else {
                                  return 1;
                              }
                          }
                      } else {
                          if (features[1] < -5) {
                              return 1;
                          } else {
                              if (features[1] < -3) {
                                  return 0;
                              } else {
                                  if (features[2] < -14) {
                                      return 1;
                                  } else {
                                      return 0;
                                  }
                              }
                          }
                      }
                  } else {
                      if (features[2] < 33) {
                          if (features[0] < 11) {
                              if (features[1] < -10) {
                                  return 1;
                              } else {
                                  if (features[2] < 22) {
                                      return 1;
                                  } else {
                                      return 1;
                                  }
                              }
                          } else {
                              return 1;
                          }
                      } else {
                          if (features[0] < 15) {
                              return 0;
                          } else {
                              return 1;
                          }
                      }
                  }
              }
          } else {
              if (features[2] < 4) {
                  if (features[1] < 17) {
                      if (features[0] < -10) {
                          return 0;
                      } else {
                          if (features[2] < -14) {
                              if (features[1] < 4) {
                                  return 1;
                              } else {
                                  if (features[2] < -24) {
                                      if (features[1] < 7) {
                                          return 1;
                                      } else {
                                          return 1;
                                      }
                                  } else {
                                      return 1;
                                  }
                              }
                          } else {
                              if (features[1] < 6) {
                                  if (features[2] < -11) {
                                      return 1;
                                  } else {
                                      if (features[2] < -8) {
                                          return 1;
                                      } else {
                                          return 1;
                                      }
                                  }
                              } else {
                                  return 0;
                              }
                          }
                      }
                  } else {
                      if (features[1] < 35) {
                          return 0;
                      } else {
                          return 0;
                      }
                  }
              } else {
                  if (features[1] < 15) {
                      if (features[0] < 0) {
                          return 1;
                      } else {
                          return 1;
                      }
                  } else {
                      return 1;
                  }
              }
          }
        }
        

int32_t testModel_predict(const int16_t *features, int32_t features_length) {

        int32_t votes[2] = {0,};
        int32_t _class = -1;

        _class = testModel_tree_0(features, features_length); votes[_class] += 1;
    
        int32_t most_voted_class = -1;
        int32_t most_voted_votes = 0;
        for (int32_t i=0; i<2; i++) {

            if (votes[i] > most_voted_votes) {
                most_voted_class = i;
                most_voted_votes = votes[i];
            }
        }
        return most_voted_class;
    }
    