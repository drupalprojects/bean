<?php

class BeanInlineEntityFormController extends EntityInlineEntityFormController {
  /**
   * Returns the entity form to be shown through the IEF widget.
   *
   * When adding data to $form_state it should be noted that there can be
   * several IEF widgets on one master form, each with several form rows,
   * leading to possible key collisions if the keys are not prefixed with
   * $entity_form['#parents'].
   *
   * @param $entity_form
   *   The entity form.
   * @param $form_state
   *   The form state of the parent form.
   */
  public function entityForm($entity_form, &$form_state) {
    module_load_include('inc', 'bean', 'includes/bean.pages.inc');

    return drupal_get_form('bean_form', $entity_form['#entity']);
  }

  /**
   * Validates the entity form.
   *
   * @param $entity_form
   *   The entity form.
   * @param $form_state
   *   The form state of the parent form.
   */
  public function entityFormValidate($entity_form, &$form_state) {
    $info = entity_get_info($this->entityType);
    $entity = $entity_form['#entity'];

    if (!empty($info['fieldable'])) {
      field_attach_form_validate($this->entityType, $entity, $entity_form, $form_state, LANGUAGE_NONE);
    }
  }
}
