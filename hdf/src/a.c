    /* search through the GR group for raster images & global attributes */
    curr_image = 0;
    if ((gr_ref = (uint16)Vfind(file_id, GR_NAME)) != 0) {
        int32 gr_key; /* Vgroup key of the GR Vgroup */

        gr_ptr->gr_ref = gr_ref; /* squirrel this away for later use */
        if ((gr_key = Vattach(file_id, (int32)gr_ref, "r")) != FAIL) {
            int32 nobjs = Vntagrefs(gr_key); /* The number of objects in the Vgroup */
            int32  img_key;                   /* Vgroup key of an image */
            int32  grp_tag, grp_ref;          /* a tag/ref in the Vgroup */
            int32  img_tag, img_ref;          /* image tag/ref in the Vgroup */
            char  *textbuf = NULL;            /* buffer to store the name in */
            size_t buf_size = 0;              /* size of the name buffer */

            for (i = 0; i < nobjs; i++) {
                if (Vgettagref(gr_key, i, &grp_tag, &grp_ref) == FAIL)
                    continue;

                switch (grp_tag) {
                    case DFTAG_VG: /* should be an image */
                        if ((img_key = Vattach(file_id, grp_ref, "r")) != FAIL) {
                            size_t buf_size = 0;

                            /* If unable to get class len, release vg, move on to next vg */
                            if (Vgetclass(img_key, NULL, &buf_size) == FAIL)
                            {
                                Vdetach(img_key);
                                continue;
                            }
                            textbuf = (char *)malloc(sizeof(char) * (buf_size + 1));
                            if (Vgetclass(img_key, textbuf, &buf_size) != FAIL) {
                                if (!strcmp(textbuf, RI_NAME)) { /* it is an image, get the image's tag/ref */
                                    for (j = 0; j < Vntagrefs(img_key); j++) {
                                        if (Vgettagref(img_key, j, &img_tag, &img_ref) == FAIL) {
                                            free(textbuf);
                                            continue;
                                        }
                                        /* Make sure the tag is correct, then
                                           store the image's info and the
                                           tag/ref of the vgroup that represents
                                           the image into image_info_struct and
                                           increment image count */
                                        if (img_tag == DFTAG_RI || img_tag == DFTAG_CI) {
                                            Store_imginfo(&img_info[curr_image], (uint16)grp_tag,
                                                          (uint16)grp_ref, (uint16)img_tag, (uint16)img_ref);
                                            img_info[curr_image].offset = Hoffset(
                                                file_id, (uint16)img_tag, (uint16)img_ref); /* store offset */
                                            curr_image++;
                                            break;
                                        } /* end if */
                                   }     /* end for */
                                }         /* end if */
                            }             /* end if */
                            Vdetach(img_key);
                        }      /* end if */
                        break; /* case DFTAG_VG, an image */

                    case DFTAG_VH: /* must be a "global" attribute */
                    {
                        at_info_t *new_attr; /* attr to add to the set of global attrs */
                        int32      at_key;   /* VData key for the attribute */

                        if ((new_attr = (at_info_t *)malloc(sizeof(at_info_t))) == NULL)
                            HGOTO_ERROR(DFE_NOSPACE, FAIL);
                        new_attr->ref           = (uint16)grp_ref;
                        new_attr->index         = gr_ptr->gattr_count;
                        new_attr->data_modified = FALSE;
                        new_attr->new_at        = FALSE;
                        new_attr->data          = NULL;

                        /* Open the vdata to read the attr's info */
                        if ((at_key = VSattach(file_id, (int32)grp_ref, "r")) != FAIL) {
                            char *fname;

                            /* Make certain the attribute only has one field */
                            if (VFnfields(at_key) != 1) {
                                VSdetach(at_key);
                                free(new_attr);
                                break;
                            }
                           new_attr->nt  = VFfieldtype(at_key, 0);
                            new_attr->len = VFfieldorder(at_key, 0);
                            if (new_attr->len == 1)
                                new_attr->len = VSelts(at_key);

                            /* Get the name of the attribute */
                            if ((fname = VFfieldname(at_key, 0)) == NULL) {
                                sprintf(textbuf, "Attribute #%d", (int)new_attr->index);
                                if ((new_attr->name = (char *)malloc(strlen(textbuf) + 1)) == NULL) {
                                    VSdetach(at_key);
                                    free(new_attr);
                                    HGOTO_ERROR(DFE_NOSPACE, FAIL);
                                }
                                strcpy(new_attr->name, textbuf);
                            }
                            else {
                                if ((new_attr->name = (char *)malloc(strlen(fname) + 1)) == NULL) {
                                    VSdetach(at_key);
                                    free(new_attr);
                                    HGOTO_ERROR(DFE_NOSPACE, FAIL);
                                }
                                strcpy(new_attr->name, fname);
                            }

                            /* insert the attr instance in B-tree */
                            tbbtdins(gr_ptr->gattree, new_attr, NULL);

                            VSdetach(at_key);
                        } /* end if */

                        /* increment the number of GR global attributes */
                        gr_ptr->gattr_count++;
                    } /* end case DFTAG_VH, a global attribute */
                    break;

                    default:
                        break;
                } /* end switch */
            }     /* end for */
            free(textbuf);
            Vdetach(gr_key);
        } /* end if */
    }     /* end if */

