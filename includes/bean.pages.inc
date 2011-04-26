<?php

/**
 * @file
 * Bean Functions
 */

/**
 * Edit bean page callback
 */
function bean_edit($bean) {
  return drupal_get_form('bean_form', $bean);
}

/**
 * Add bean page callback
 */
function bean_add($type) {
  $bean = bean_create(array('type' => $type));

  return drupal_get_form('bean_form', $bean);
}

/**
 * Menu Callback to list available bean types to add
 */
function bean_add_page() {
  $bean_types = bean_get_types();
  // Bypass the block/add listing if only one block type is available.
  if (count($bean_types) == 1) {
    $bean = array_shift($bean_types);
    drupal_goto($bean->addURL());
  }

  return theme('bean_add_list', array('content' => $bean_types));
}

/**
 * Returns HTML for a list of available bean types for bean creation.
 *
 * @param $variables
 *   An associative array containing:
 *   - content: An array of bean types.
 *
 * @ingroup themeable
 */
function theme_bean_add_list($variables) {
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="bean-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l('Add ' . $item->label, $item->addURL()) . '</dt>';
    }
    $output .= '</dl>';
  }
  else {
    $output = '<p>' . t('You have not created any block types yet. Go to the <a href="@create-content">block type creation page</a> to add a new bean type.', array('@create-content' => url('admin/structure/beans/add'))) . '</p>';
  }
  return $output;
}

/**
 * Bean form
 */
function bean_form($form, &$form_state, $bean) {
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#required' => TRUE,
    '#default_value' => $bean->label,
  );

  $form['bean'] = array(
    '#type' => 'value',
    '#value' => $bean,
  );

  field_attach_form('bean', $bean, $form, $form_state);

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 200,
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Validation for bean form
 */
function bean_form_validate($form, &$form_state) {
  $bean = $form_state['values']['bean'];
  $bean->label = $form_state['values']['label'];

  field_attach_form_validate('bean', $bean, $form, $form_state);
  $form_state['values']['bean'] = $bean;
}

/**
 * Submit function for bean form
 */
function bean_form_submit($form, &$form_state) {
  $bean = $form_state['values']['bean'];
  field_attach_submit('bean', $bean, $form, $form_state);
  $bean->save();
  $form_state['redirect'] = $bean->editURL();
}