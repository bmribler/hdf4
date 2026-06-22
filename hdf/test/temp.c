statusint = Vgetname(vg1, NULL, &buf_size);
CHECK(statusint, FAIL, "Vgetname");
VERIFY(buf_size, strlen(VG_LONGNAME), "Vgetname");

vgname = (char *)malloc(sizeof(char) * (buf_size + 1));
CHECK_ALLOC(vgname, "vgname", "test_vglongnames");

statusint = Vgetname(vg1, vgname, &buf_size);
CHECK(statusint, FAIL, "Vgetname");
VERIFY(buf_size, strlen(VG_LONGNAME), "Vgetname");
if (strcmp(vgname, VG_LONGNAME)) {
    num_errs++;
    printf(">>> Got bogus Vgroup name : %s\n", vgname);
}

size_t buf_size = 0;
if (Vgetclass(img_key, NULL, &buf_size) == FAIL)
    continue;
textbuf = (char *)malloc(sizeof(char) * (buf_size + 1));
if (Vgetclass(img_key, textbuf, &buf_size) != FAIL) {
